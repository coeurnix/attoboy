#include "attowebrequest_internal.h"
#include "attostring_internal.h"

namespace attoboy {

static WebResponseImpl *
PerformRequest(WebRequestImpl *reqImpl, const WCHAR *method,
               const unsigned char *body, int bodySize, int timeout) {
  if (!reqImpl || !reqImpl->url)
    return nullptr;

  WebResponseImpl *respImpl =
      (WebResponseImpl *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   sizeof(WebResponseImpl));
  if (!respImpl)
    return nullptr;

  InitializeSRWLock(&respImpl->lock);
  respImpl->refCount = 1;
  respImpl->headers = new Map();

  URL_COMPONENTS urlComp;
  ZeroMemory(&urlComp, sizeof(urlComp));
  urlComp.dwStructSize = sizeof(urlComp);

  WCHAR *hostname = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 256 * sizeof(WCHAR));
  WCHAR *urlPath = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 2048 * sizeof(WCHAR));
  if (!hostname || !urlPath) {
    if (hostname) HeapFree(GetProcessHeap(), 0, hostname);
    if (urlPath) HeapFree(GetProcessHeap(), 0, urlPath);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }
  urlComp.lpszHostName = hostname;
  urlComp.dwHostNameLength = 256;
  urlComp.lpszUrlPath = urlPath;
  urlComp.dwUrlPathLength = 2048;

  if (!WinHttpCrackUrl(reqImpl->url, 0, 0, &urlComp)) {
    HeapFree(GetProcessHeap(), 0, hostname);
    HeapFree(GetProcessHeap(), 0, urlPath);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  HINTERNET hSession =
      WinHttpOpen(L"libattoboy/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                  WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  if (!hSession) {
    HeapFree(GetProcessHeap(), 0, hostname);
    HeapFree(GetProcessHeap(), 0, urlPath);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  if (timeout > 0) {
    WinHttpSetTimeouts(hSession, timeout, timeout, timeout, timeout);
  }

  HINTERNET hConnect = WinHttpConnect(hSession, hostname, urlComp.nPort, 0);
  if (!hConnect) {
    WinHttpCloseHandle(hSession);
    HeapFree(GetProcessHeap(), 0, hostname);
    HeapFree(GetProcessHeap(), 0, urlPath);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
                    ? WINHTTP_FLAG_SECURE
                    : 0;
  HINTERNET hRequest =
      WinHttpOpenRequest(hConnect, method, urlPath, nullptr,
                         WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
  if (!hRequest) {
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    HeapFree(GetProcessHeap(), 0, hostname);
    HeapFree(GetProcessHeap(), 0, urlPath);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  HeapFree(GetProcessHeap(), 0, hostname);
  HeapFree(GetProcessHeap(), 0, urlPath);

  String headersStr;
  if (reqImpl->headers && !reqImpl->headers->isEmpty()) {
    List keys = reqImpl->headers->keys();
    for (int i = 0; i < keys.length(); i++) {
      String key = keys.at<String>(i);
      String value = reqImpl->headers->get<String, String>(key);
      headersStr = headersStr.append(key)
                       .append(String(": "))
                       .append(value)
                       .append(String("\r\n"));
    }
  }

  WCHAR *headersWide = nullptr;
  DWORD headersLen = WINHTTP_NO_ADDITIONAL_HEADERS;
  if (!headersStr.isEmpty()) {
    headersWide = Utf8ToWide(headersStr.c_str());
    headersLen = WINHTTP_NO_REQUEST_DATA;
  }

  BOOL sent = WinHttpSendRequest(
      hRequest, headersWide ? headersWide : WINHTTP_NO_ADDITIONAL_HEADERS,
      headersLen, (LPVOID)body, bodySize, bodySize, 0);

  if (headersWide)
    FreeConvertedString(headersWide);

  if (!sent) {
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  if (!WinHttpReceiveResponse(hRequest, nullptr)) {
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    delete respImpl->headers;
    HeapFree(GetProcessHeap(), 0, respImpl);
    return nullptr;
  }

  DWORD statusCode = 0;
  DWORD statusCodeSize = sizeof(statusCode);
  WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                      WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize,
                      WINHTTP_NO_HEADER_INDEX);
  respImpl->statusCode = (int)statusCode;

  WCHAR *statusText = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 256 * sizeof(WCHAR));
  if (statusText) {
    DWORD statusTextSize = 256 * sizeof(WCHAR);
    if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_TEXT,
                            WINHTTP_HEADER_NAME_BY_INDEX, statusText,
                            &statusTextSize, WINHTTP_NO_HEADER_INDEX)) {
      respImpl->statusReason = AllocWebString(lstrlenW(statusText));
      if (respImpl->statusReason)
        lstrcpyW(respImpl->statusReason, statusText);
    }
    HeapFree(GetProcessHeap(), 0, statusText);
  }

  respImpl->finalUrl = AllocWebString(lstrlenW(reqImpl->url));
  if (respImpl->finalUrl)
    lstrcpyW(respImpl->finalUrl, reqImpl->url);

  WCHAR *allHeaders = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 8192 * sizeof(WCHAR));
  if (allHeaders) {
    DWORD allHeadersSize = 8192 * sizeof(WCHAR);
    if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                            WINHTTP_HEADER_NAME_BY_INDEX, allHeaders,
                            &allHeadersSize, WINHTTP_NO_HEADER_INDEX)) {
    char *headersUtf8 = WideToUtf8(allHeaders);
    if (headersUtf8) {
      String headersString(headersUtf8);
      List lines = headersString.lines();
      for (int i = 1; i < lines.length(); i++) {
        String line = lines.at<String>(i);
        if (line.isEmpty())
          continue;
        int colonPos = line.getPositionOf(String(":"));
        if (colonPos > 0) {
          String headerName = line.substring(0, colonPos).trim();
          String headerValue = line.substring(colonPos + 1).trim();
          respImpl->headers->put(headerName, headerValue);
        }
      }
      FreeConvertedString(headersUtf8);
    }
    }
    HeapFree(GetProcessHeap(), 0, allHeaders);
  }

  DWORD totalSize = 0;
  DWORD availableSize = 0;
  unsigned char *buffer = nullptr;

  do {
    availableSize = 0;
    if (!WinHttpQueryDataAvailable(hRequest, &availableSize))
      break;
    if (availableSize == 0)
      break;

    unsigned char *newBuffer;
    if (buffer) {
      newBuffer = (unsigned char *)HeapReAlloc(
          GetProcessHeap(), 0, buffer, totalSize + availableSize);
    } else {
      newBuffer = (unsigned char *)HeapAlloc(
          GetProcessHeap(), 0, totalSize + availableSize);
    }
    if (!newBuffer) {
      if (buffer)
        HeapFree(GetProcessHeap(), 0, buffer);
      break;
    }
    buffer = newBuffer;

    DWORD bytesRead = 0;
    if (!WinHttpReadData(hRequest, buffer + totalSize, availableSize,
                         &bytesRead))
      break;

    totalSize += bytesRead;
  } while (availableSize > 0);

  respImpl->body = buffer;
  respImpl->bodySize = totalSize;

  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);

  return respImpl;
}

WebResponse WebRequest::doGet(int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  impl->response = PerformRequest(impl, L"GET", nullptr, 0, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doPost(int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  impl->response = PerformRequest(impl, L"POST", nullptr, 0, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doPost(const Map &map, int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  if (!impl->headers->hasKey<String>(String("Content-Type"))) {
    impl->headers->put(String("Content-Type"), String("application/json"));
  }

  String jsonBody = String(map);
  const char *bodyData = jsonBody.c_str();
  int bodyLen = jsonBody.byteLength();

  impl->response = PerformRequest(impl, L"POST", (const unsigned char *)bodyData,
                                  bodyLen, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doPost(const List &list, int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  if (!impl->headers->hasKey<String>(String("Content-Type"))) {
    impl->headers->put(String("Content-Type"), String("application/json"));
  }

  String jsonBody = String(list);
  const char *bodyData = jsonBody.c_str();
  int bodyLen = jsonBody.byteLength();

  impl->response = PerformRequest(impl, L"POST", (const unsigned char *)bodyData,
                                  bodyLen, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doPost(const Buffer &buf, int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);

  impl->response =
      PerformRequest(impl, L"POST", data, len, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doPost(const String &str, int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  const char *data = str.c_str();
  int len = str.byteLength();

  impl->response =
      PerformRequest(impl, L"POST", (const unsigned char *)data, len, timeout);
  impl->hasCompleted = true;

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

WebResponse WebRequest::doRequest(const String &method,
                                  const Buffer &buf, int timeout) {
  if (!impl)
    return WebResponse();

  WriteLockGuard lock(&impl->lock);
  if (impl->hasCompleted)
    return WebResponse();

  WCHAR *methodWide = Utf8ToWide(method.c_str());
  if (!methodWide)
    return WebResponse();

  int len = 0;
  const unsigned char *data = buf.c_ptr(&len);

  impl->response = PerformRequest(impl, methodWide, data, len, timeout);
  impl->hasCompleted = true;

  FreeConvertedString(methodWide);

  if (!impl->response)
    return WebResponse();

  WebResponse resp;
  resp.impl = impl->response;
  InterlockedIncrement(&impl->response->refCount);
  return resp;
}

} // namespace attoboy
