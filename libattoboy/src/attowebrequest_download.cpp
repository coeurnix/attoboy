#include "attowebrequest_internal.h"
#include "attopath_internal.h"

namespace attoboy {

bool WebRequest::Download(const String &url, const String &savePath,
                          const Map *params, const Map *headers,
                          bool overwrite, int timeout) {
  Path filePath(savePath);

  if (filePath.exists() && !overwrite)
    return false;

  WebRequest req(url, params, headers);
  const WebResponse *resp = req.doGet(timeout);

  if (!resp || !resp->succeeded()) {
    if (resp)
      delete resp;
    return false;
  }

  Buffer data = resp->asBuffer();
  delete resp;

  bool success = filePath.writeFromBuffer(data);
  return success;
}

} // namespace attoboy
