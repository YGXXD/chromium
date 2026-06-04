// Copyright 2026 YGXXD

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_FRAME_DATA_PROVIDER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_FRAME_DATA_PROVIDER_H_

#include "cc/paint/skottie_frame_data_provider.h"
#include "third_party/blink/renderer/modules/canvas/canvas2d/xd_lottie_image_loader.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {
class XdLottieImageAsset;

class XdLottieFrameDataProvider
    : public GarbageCollected<XdLottieFrameDataProvider>,
      public XdLottieImageLoaderClient,
      public cc::SkottieFrameDataProvider {
 public:
  explicit XdLottieFrameDataProvider(LocalDOMWindow& local_dom_window);
  XdLottieFrameDataProvider(const XdLottieFrameDataProvider&) = delete;
  XdLottieFrameDataProvider& operator=(const XdLottieFrameDataProvider&) =
      delete;
  ~XdLottieFrameDataProvider() override;

  scoped_refptr<cc::SkottieFrameDataProvider::ImageAsset> LoadImageAsset(
      std::string_view resource_id,
      const base::FilePath& resource_path,
      const std::optional<gfx::Size>& size) override;

  void ImageNotifyFinished(const std::string& resource_id,
                           scoped_refptr<SharedBuffer> data) override;
  void Trace(Visitor* visitor) const override;

  void ReloadAssets(String assets_path);
  String AssetPath() { return assets_path_; }

 private:
  base::flat_map<std::string, scoped_refptr<XdLottieImageAsset>>
      current_assets_;
  Member<LocalDOMWindow> local_dom_window_;
  HeapVector<Member<XdLottieImageLoader>> image_loaders_;
  String assets_path_;
};

}  // namespace blink

#endif
