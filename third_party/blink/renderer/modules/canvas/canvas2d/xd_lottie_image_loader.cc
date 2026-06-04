// Copyright 2026 YGXXD

#include "third_party/blink/renderer/modules/canvas/canvas2d/xd_lottie_image_loader.h"

#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-data-view.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/platform/loader/fetch/fetch_parameters.h"

namespace blink {
XdLottieImageLoader::XdLottieImageLoader(const std::string& resource_id,
                                         const std::string& resource_path,
                                         XdLottieImageLoaderClient& client)
    : resource_id_(resource_id),
      resource_path_(resource_path),
      client_(client) {}

void XdLottieImageLoader::FetchData(LocalDOMWindow* local_dom,
                                    const std::string& assets_path) {
  ResourceFetcher* fetcher = local_dom->Fetcher();
  if (fetcher) {
    std::string url = assets_path + resource_path_;
    ResourceRequest request(url.c_str());
    request.SetPriority(WebURLRequest::Priority::kHigh);
    request.SetReferrerPolicy(local_dom->GetReferrerPolicy());
    if (local_dom->GetExecutionContext()) {
      const SecurityOrigin* security_origin =
          local_dom->GetExecutionContext()->GetSecurityOrigin();
      if (security_origin) {
        request.SetRequestorOrigin(security_origin);
      }
    }
    request.SetReferrerString(local_dom->document()->referrer());
    request.SetRequestContext(blink::mojom::RequestContextType::SCRIPT);

    FetchParameters params(std::move(request), ResourceLoaderOptions(nullptr));
    resource_ = RawResource::Fetch(params, fetcher, this);
  }
}

void XdLottieImageLoader::NotifyFinished(Resource* resource) {
  if (!resource || resource->ErrorOccurred() || !resource->ResourceBuffer()) {
    return;
  }
  if (client_) {
    scoped_refptr<const SharedBuffer> const_data = resource->ResourceBuffer();
    scoped_refptr<SharedBuffer> data =
        base::WrapRefCounted(const_cast<SharedBuffer*>(const_data.get()));
    client_->ImageNotifyFinished(resource_id_, std::move(data));
  }
}

void XdLottieImageLoader::Trace(Visitor* visitor) const {
  visitor->Trace(client_);
  visitor->Trace(resource_);
  RawResourceClient::Trace(visitor);
}
}  // namespace blink
