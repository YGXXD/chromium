// Copyright 2026 YGXXD

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_CANVAS2D_XD_LOTTIE_H_

#include "base/memory/scoped_refptr.h"
#include "cc/paint/skottie_frame_data_provider.h"
#include "cc/paint/skottie_wrapper.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {
class LocalDOMWindow;
class XdLottieFrameDataProvider;

class CORE_EXPORT XdLottie : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static XdLottie* Create(LocalDOMWindow& window, String lottie_data);
  explicit XdLottie(LocalDOMWindow& window, String lottie_data);
  ~XdLottie() override;

  void Trace(Visitor* visitor) const override;

  void LoadAllFrameData(float t, cc::SkottieFrameDataMap& all_frame_data);
  scoped_refptr<cc::SkottieWrapper> skottie() const { return skottie_; }

  void setAssetsPath(String asset_path);
  String assetsPath() const;
  float width() const { return skottie_->size().width(); }
  float height() const { return skottie_->size().height(); }
  float duration() const { return skottie_->duration(); }

 private:
  cc::SkottieWrapper::FrameDataFetchResult LoadImageForAsset(
      cc::SkottieFrameDataMap& all_frame_data,
      cc::SkottieResourceIdHash asset_id,
      float t,
      sk_sp<SkImage>&,
      SkSamplingOptions&);

  base::flat_map<cc::SkottieResourceIdHash,
                 scoped_refptr<cc::SkottieFrameDataProvider::ImageAsset>>
      image_assets_;
  scoped_refptr<cc::SkottieWrapper> skottie_;
  Member<XdLottieFrameDataProvider> frame_data_provider_;
  Member<LocalDOMWindow> local_dom_window_;
};

}  // namespace blink

#endif
