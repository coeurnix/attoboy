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
  WebResponse resp = req.doGet(timeout);

  if (!resp.succeeded()) {
    return false;
  }

  Buffer data = resp.asBuffer();

  bool success = filePath.writeFromBuffer(data);
  return success;
}

} // namespace attoboy
