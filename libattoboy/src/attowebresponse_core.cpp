#include "attowebrequest_internal.h"
#include "attostring_internal.h"

namespace attoboy {

WebResponse::WebResponse() {
  impl = (WebResponseImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                      sizeof(WebResponseImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);
  impl->refCount = 1;
  impl->statusCode = 0;
  impl->statusReason = nullptr;
  impl->finalUrl = nullptr;
  impl->body = nullptr;
  impl->bodySize = 0;
  impl->headers = nullptr;
  impl->jsonCache = nullptr;
}

WebResponse::WebResponse(const WebResponse &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

WebResponse::~WebResponse() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      FreeWebString(impl->statusReason);
      FreeWebString(impl->finalUrl);
      if (impl->body)
        HeapFree(GetProcessHeap(), 0, impl->body);
      if (impl->headers)
        delete impl->headers;
      if (impl->jsonCache)
        delete impl->jsonCache;
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

WebResponse &WebResponse::operator=(const WebResponse &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      FreeWebString(impl->statusReason);
      FreeWebString(impl->finalUrl);
      if (impl->body)
        HeapFree(GetProcessHeap(), 0, impl->body);
      if (impl->headers)
        delete impl->headers;
      if (impl->jsonCache)
        delete impl->jsonCache;
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

bool WebResponse::succeeded() const {
  if (!impl)
    return false;
  ReadLockGuard lock(&impl->lock);
  return impl->statusCode >= 200 && impl->statusCode < 300;
}

int WebResponse::getStatusCode() const {
  if (!impl)
    return 0;
  ReadLockGuard lock(&impl->lock);
  return impl->statusCode;
}

String WebResponse::getStatusReason() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->statusReason)
    return String();
  return String(WideToUtf8(impl->statusReason));
}

String WebResponse::getUrl() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->finalUrl)
    return String();
  char *utf8Url = WideToUtf8(impl->finalUrl);
  String result(utf8Url);
  FreeConvertedString(utf8Url);
  return result;
}

Map WebResponse::getResponseHeaders() const {
  if (!impl)
    return Map();
  ReadLockGuard lock(&impl->lock);
  if (!impl->headers)
    return Map();
  return *impl->headers;
}

const Map *WebResponse::asJson() const {
  if (!impl)
    return nullptr;
  WriteLockGuard lock(&impl->lock);

  if (impl->jsonCache)
    return impl->jsonCache;

  if (!impl->body || impl->bodySize == 0)
    return nullptr;

  String jsonStr((const char *)impl->body, impl->bodySize);
  Map parsed = Map::FromJSONString(jsonStr);

  if (parsed.isEmpty())
    return nullptr;

  impl->jsonCache = new Map(parsed);
  return impl->jsonCache;
}

String WebResponse::asString() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->body || impl->bodySize == 0)
    return String();
  return String((const char *)impl->body, impl->bodySize);
}

Buffer WebResponse::asBuffer() const {
  if (!impl)
    return Buffer();
  ReadLockGuard lock(&impl->lock);
  if (!impl->body || impl->bodySize == 0)
    return Buffer();
  return Buffer(impl->body, impl->bodySize);
}

} // namespace attoboy
