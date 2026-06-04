// Copyright 2026 YGXXD

#include "third_party/blink/renderer/modules/canvas/canvas2d/xd_lottie.h"

#include "cc/paint/skottie_wrapper.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/xd_lottie_frame_data_provider.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

XdLottie* XdLottie::Create(LocalDOMWindow& window, String lottie_data) {
  return MakeGarbageCollected<XdLottie>(window, std::move(lottie_data));
}

XdLottie::XdLottie(LocalDOMWindow& window, String lottie_data)
    : local_dom_window_(window) {
  std::string json_span = lottie_data.Utf8();
  skottie_ = cc::SkottieWrapper::UnsafeCreateSerializable(
      std::vector<uint8_t>(json_span.begin(), json_span.end()));

  bool has_external_image_assets =
      !skottie_->GetImageAssetMetadata().asset_storage().empty();
  if (has_external_image_assets) {
    frame_data_provider_ =
        MakeGarbageCollected<XdLottieFrameDataProvider>(*local_dom_window_);
    for (const auto& asset_metadata_pair :
         skottie_->GetImageAssetMetadata().asset_storage()) {
      const std::string& asset_id = asset_metadata_pair.first;
      const auto& asset_metadata = asset_metadata_pair.second;
      const base::FilePath& resource_path = asset_metadata.resource_path;
      scoped_refptr<cc::SkottieFrameDataProvider::ImageAsset> new_asset =
          frame_data_provider_->LoadImageAsset(asset_id, resource_path,
                                               asset_metadata.size);
      image_assets_.emplace(cc::HashSkottieResourceId(asset_id),
                            std::move(new_asset));
    }
  }
}

XdLottie::~XdLottie() {}

void XdLottie::Trace(Visitor* visitor) const {
  ScriptWrappable::Trace(visitor);
  visitor->Trace(local_dom_window_);
  visitor->Trace(frame_data_provider_);
}

void XdLottie::LoadAllFrameData(float t,
                                cc::SkottieFrameDataMap& all_frame_data) {
  skottie_->Seek(
      t, base::BindRepeating(&XdLottie::LoadImageForAsset, WrapPersistent(this),
                             std::ref(all_frame_data)));
}

cc::SkottieWrapper::FrameDataFetchResult XdLottie::LoadImageForAsset(
    cc::SkottieFrameDataMap& all_frame_data,
    cc::SkottieResourceIdHash asset_id,
    float t,
    sk_sp<SkImage>&,
    SkSamplingOptions&) {
  cc::SkottieFrameDataProvider::ImageAsset& image_asset =
      *image_assets_.at(asset_id);
  all_frame_data.emplace(asset_id, image_asset.GetFrameData(t, 1));
  return cc::SkottieWrapper::FrameDataFetchResult::kNoUpdate;
}

void XdLottie::setAssetsPath(String asset_path) {
  frame_data_provider_->ReloadAssets(std::move(asset_path));
}

String XdLottie::assetsPath() const {
  return frame_data_provider_->AssetPath();
}

}  // namespace blink
