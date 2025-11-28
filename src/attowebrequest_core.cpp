#include "attowebrequest_internal.h"
#include "attostring_internal.h"

namespace attoboy {

static String BuildUrlWithParams(const String &baseUrl, const Map &params) {
  if (params.isEmpty())
    return baseUrl;

  String url = baseUrl;
  bool hasQuery = url.contains(String("?"));

  List keys = params.keys();
  for (int i = 0; i < keys.length(); i++) {
    String key = keys.at<String>(i);
    String value = params.get<String, String>(key);

    if (i == 0 && !hasQuery)
      url = url.append(String("?"));
    else
      url = url.append(String("&"));

    url = url.append(key).append(String("=")).append(value);
  }

  return url;
}

WebRequest::WebRequest(const String &url, const Map &params,
                       const Map &headers) {
  impl = (WebRequestImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                     sizeof(WebRequestImpl));
  if (!impl)
    return;
  InitializeSRWLock(&impl->lock);
  impl->refCount = 1;
  impl->hasCompleted = false;
  impl->response = nullptr;

  String fullUrl = BuildUrlWithParams(url, params);
  impl->url = Utf8ToWide(fullUrl.c_str());

  if (!params.isEmpty())
    impl->params = new Map(params);
  else
    impl->params = new Map();

  if (!headers.isEmpty())
    impl->headers = new Map(headers);
  else
    impl->headers = new Map();
}

WebRequest::WebRequest(const WebRequest &other) {
  impl = other.impl;
  if (impl) {
    InterlockedIncrement(&impl->refCount);
  }
}

WebRequest::~WebRequest() {
  if (impl) {
    if (InterlockedDecrement(&impl->refCount) == 0) {
      FreeWebString(impl->url);
      if (impl->params)
        delete impl->params;
      if (impl->headers)
        delete impl->headers;
      if (impl->response) {
        if (InterlockedDecrement(&impl->response->refCount) == 0) {
          FreeWebString(impl->response->statusReason);
          FreeWebString(impl->response->finalUrl);
          if (impl->response->body)
            HeapFree(GetProcessHeap(), 0, impl->response->body);
          if (impl->response->headers)
            delete impl->response->headers;
          HeapFree(GetProcessHeap(), 0, impl->response);
        }
      }
      HeapFree(GetProcessHeap(), 0, impl);
    }
  }
}

WebRequest &WebRequest::operator=(const WebRequest &other) {
  if (this != &other) {
    if (impl && InterlockedDecrement(&impl->refCount) == 0) {
      FreeWebString(impl->url);
      if (impl->params)
        delete impl->params;
      if (impl->headers)
        delete impl->headers;
      if (impl->response) {
        if (InterlockedDecrement(&impl->response->refCount) == 0) {
          FreeWebString(impl->response->statusReason);
          FreeWebString(impl->response->finalUrl);
          if (impl->response->body)
            HeapFree(GetProcessHeap(), 0, impl->response->body);
          if (impl->response->headers)
            delete impl->response->headers;
          HeapFree(GetProcessHeap(), 0, impl->response);
        }
      }
      HeapFree(GetProcessHeap(), 0, impl);
    }
    impl = other.impl;
    if (impl) {
      InterlockedIncrement(&impl->refCount);
    }
  }
  return *this;
}

String WebRequest::getUrl() const {
  if (!impl)
    return String();
  ReadLockGuard lock(&impl->lock);
  if (!impl->url)
    return String();
  char *utf8Url = WideToUtf8(impl->url);
  String result(utf8Url);
  FreeConvertedString(utf8Url);
  return result;
}

Map WebRequest::getParams() const {
  if (!impl)
    return Map();
  ReadLockGuard lock(&impl->lock);
  if (!impl->params)
    return Map();
  return *impl->params;
}

Map WebRequest::getHeaders() const {
  if (!impl)
    return Map();
  ReadLockGuard lock(&impl->lock);
  if (!impl->headers)
    return Map();
  return *impl->headers;
}

bool WebRequest::hasCompleted() const {
  if (!impl)
    return true;
  ReadLockGuard lock(&impl->lock);
  return impl->hasCompleted;
}

} // namespace attoboy
