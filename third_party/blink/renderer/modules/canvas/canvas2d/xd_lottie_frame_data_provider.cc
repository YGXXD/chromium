// Copyright 2026 YGXXD

#include "third_party/blink/renderer/modules/canvas/canvas2d/xd_lottie_frame_data_provider.h"

#include "cc/paint/paint_image_builder.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/platform/graphics/deferred_image_decoder.h"

namespace blink {
class XdLottieImageAsset : public cc::SkottieFrameDataProvider::ImageAsset {
 public:
  explicit XdLottieImageAsset() = default;
  XdLottieImageAsset(const XdLottieImageAsset& other) = delete;
  XdLottieImageAsset& operator=(const XdLottieImageAsset& other) = delete;
  ~XdLottieImageAsset() override = default;

  cc::SkottieFrameData GetFrameData(float t, float scale_factor) override {
    return current_frame_data_;
  }

  void SetImageBuffer(scoped_refptr<SharedBuffer> data) {
    decoder_ = blink::DeferredImageDecoder::Create(
        data, true, ImageDecoder::kAlphaPremultiplied, ColorBehavior::kTag);
    if (decoder_) {
      cc::PaintImageBuilder builder =
          cc::PaintImageBuilder::WithDefault()
              .set_id(cc::PaintImage::GetNextId())
              .set_paint_image_generator(decoder_->CreateGenerator())
              .set_is_high_bit_depth(decoder_->ImageIsHighBitDepth())
              .set_decoding_mode(cc::PaintImage::DecodingMode::kAsync)
              .set_completion_state(
                  cc::PaintImage::CompletionState::kPartiallyDone);
      sk_sp<PaintImageGenerator> gainmap_generator;
      SkGainmapInfo gainmap_info;
      if (decoder_->CreateGainmapGenerator(gainmap_generator, gainmap_info)) {
        DCHECK(gainmap_generator);
        builder = builder.set_gainmap_paint_image_generator(
            std::move(gainmap_generator), gainmap_info);
      }
      current_frame_data_.image = builder.TakePaintImage();
      current_frame_data_.quality = cc::PaintFlags::FilterQuality::kHigh;
    }
  }

 private:
  std::unique_ptr<DeferredImageDecoder> decoder_;
  cc::SkottieFrameData current_frame_data_;
};

XdLottieFrameDataProvider::XdLottieFrameDataProvider(
    LocalDOMWindow& local_dom_window)
    : local_dom_window_(local_dom_window) {}

XdLottieFrameDataProvider::~XdLottieFrameDataProvider() = default;

scoped_refptr<cc::SkottieFrameDataProvider::ImageAsset>
XdLottieFrameDataProvider::LoadImageAsset(
    std::string_view resource_id,
    const base::FilePath& resource_path,
    const std::optional<gfx::Size>& size) {
  const std::string& path_string = resource_path.value();
  scoped_refptr<XdLottieImageAsset> image_asset =
      base::MakeRefCounted<XdLottieImageAsset>();
  CHECK(current_assets_.emplace(std::string(resource_id), image_asset).second);
  std::string image_url = assets_path_.Utf8() + path_string;
  XdLottieImageLoader* image_loader = MakeGarbageCollected<XdLottieImageLoader>(
      std::string(resource_id), image_url, *this);
  image_loaders_.emplace_back(image_loader);
  return image_asset;
}

void XdLottieFrameDataProvider::ImageNotifyFinished(
    const std::string& resource_id,
    scoped_refptr<SharedBuffer> data) {
  current_assets_.at(resource_id)->SetImageBuffer(std::move(data));
}

void XdLottieFrameDataProvider::Trace(Visitor* visitor) const {
  visitor->Trace(local_dom_window_);
  visitor->Trace(image_loaders_);
}

void XdLottieFrameDataProvider::ReloadAssets(String assets_path) {
  assets_path_ = assets_path;
  std::string assets_path_string = assets_path.Utf8();
  for (auto& loader : image_loaders_) {
    loader->FetchData(local_dom_window_, assets_path_string);
  }
}

}  // namespace blink
