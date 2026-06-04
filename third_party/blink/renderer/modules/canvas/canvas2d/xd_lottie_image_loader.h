// Copyright 2026 YGXXD

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_IMAGE_LOADER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_IMAGE_LOADER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/loader/fetch/raw_resource.h"

namespace blink {
class LocalDOMWindow;

class XdLottieImageLoaderClient : public GarbageCollectedMixin {
 public:
  virtual void ImageNotifyFinished(const std::string& id,
                                   scoped_refptr<SharedBuffer> data) = 0;
};

class CORE_EXPORT XdLottieImageLoader
    : public GarbageCollected<XdLottieImageLoader>,
      public RawResourceClient {
 public:
  XdLottieImageLoader(const std::string& resource_id,
                      const std::string& resource_path,
                      XdLottieImageLoaderClient& client);
  ~XdLottieImageLoader() override {}

  void FetchData(LocalDOMWindow* local_dom, const std::string& assets_path);
  void NotifyFinished(Resource* resource) override;

  void Trace(Visitor* visitor) const override;
  String DebugName() const override { return "XdLottieImageAsset"; }

 private:
  std::string resource_id_;
  std::string resource_path_;
  Member<XdLottieImageLoaderClient> client_;
  Member<Resource> resource_;
};

}  // namespace blink

#endif
