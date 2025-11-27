//==============================================================================
// Attoboy HTTP File Server
// A simple single-threaded HTTP server demonstrating attoboy's networking
//==============================================================================
//
// Usage:
//   attoboy_httpd                     # Serve current directory on port 8123
//   attoboy_httpd -p 8080             # Serve current directory on port 8080
//   attoboy_httpd C:\www              # Serve C:\www on port 8123
//   attoboy_httpd -p 80 C:\www        # Serve C:\www on port 80
//
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

//------------------------------------------------------------------------------
// Content-Type Mapping
//------------------------------------------------------------------------------

static String GetContentType(const String &extension) {
  String ext = extension.lower();

  // Text formats
  if (ext == "html" || ext == "htm")
    return "text/html; charset=utf-8";
  if (ext == "css")
    return "text/css; charset=utf-8";
  if (ext == "js")
    return "text/javascript; charset=utf-8";
  if (ext == "json")
    return "application/json; charset=utf-8";
  if (ext == "xml")
    return "application/xml; charset=utf-8";
  if (ext == "txt")
    return "text/plain; charset=utf-8";
  if (ext == "csv")
    return "text/csv; charset=utf-8";
  if (ext == "md")
    return "text/markdown; charset=utf-8";

  // Images
  if (ext == "png")
    return "image/png";
  if (ext == "jpg" || ext == "jpeg")
    return "image/jpeg";
  if (ext == "gif")
    return "image/gif";
  if (ext == "ico")
    return "image/x-icon";
  if (ext == "svg")
    return "image/svg+xml";
  if (ext == "webp")
    return "image/webp";
  if (ext == "bmp")
    return "image/bmp";

  // Fonts
  if (ext == "woff")
    return "font/woff";
  if (ext == "woff2")
    return "font/woff2";
  if (ext == "ttf")
    return "font/ttf";
  if (ext == "otf")
    return "font/otf";
  if (ext == "eot")
    return "application/vnd.ms-fontobject";

  // Documents
  if (ext == "pdf")
    return "application/pdf";
  if (ext == "zip")
    return "application/zip";
  if (ext == "gz" || ext == "gzip")
    return "application/gzip";
  if (ext == "tar")
    return "application/x-tar";

  // Audio/Video
  if (ext == "mp3")
    return "audio/mpeg";
  if (ext == "wav")
    return "audio/wav";
  if (ext == "ogg")
    return "audio/ogg";
  if (ext == "mp4")
    return "video/mp4";
  if (ext == "webm")
    return "video/webm";

  // Default binary
  return "application/octet-stream";
}

//------------------------------------------------------------------------------
// HTTP Response Helpers
//------------------------------------------------------------------------------

static String GetStatusText(int code) {
  if (code == 200)
    return "OK";
  if (code == 400)
    return "Bad Request";
  if (code == 403)
    return "Forbidden";
  if (code == 404)
    return "Not Found";
  if (code == 405)
    return "Method Not Allowed";
  if (code == 500)
    return "Internal Server Error";
  return "Unknown";
}

static String BuildResponseHeaders(int statusCode, const String &contentType,
                                   long long contentLength) {
  String response =
      String("HTTP/1.1 ", statusCode, " ", GetStatusText(statusCode), "\r\n");
  response = response + "Content-Type: " + contentType + "\r\n";
  response =
      response + "Content-Length: " + String((int)contentLength) + "\r\n";
  response = response + "Connection: close\r\n";
  response = response + "Server: attoboy-httpd/1.0\r\n";
  response = response + "\r\n";
  return response;
}

static void SendErrorResponse(File &client, int statusCode) {
  String body =
      String("<!DOCTYPE html>\n"
             "<html><head><title>",
             statusCode, " ", GetStatusText(statusCode),
             "</title>\n"
             "<style>body{font-family:system-ui,sans-serif;max-width:600px;"
             "margin:80px auto;"
             "text-align:center;color:#333}h1{color:#c00}</style></head>\n"
             "<body><h1>",
             statusCode, " ", GetStatusText(statusCode),
             "</h1>\n"
             "<p>The requested resource could not be served.</p>\n"
             "<hr><p><em>attoboy-httpd</em></p></body></html>\n");

  String headers = BuildResponseHeaders(statusCode, "text/html; charset=utf-8",
                                        body.byteLength());
  client.write(headers);
  client.write(body);
}

//------------------------------------------------------------------------------
// Path Security
//------------------------------------------------------------------------------

static String NormalizePath(const String &path) {
  // Decode URL-encoded characters (basic: %20, %2F, etc.)
  String decoded;
  int len = path.byteLength();
  const char *p = path.c_str();

  for (int i = 0; i < len; ++i) {
    if (p[i] == '%' && i + 2 < len) {
      char hex[3] = {p[i + 1], p[i + 2], 0};
      int val = 0;
      for (int j = 0; j < 2; ++j) {
        char c = hex[j];
        val *= 16;
        if (c >= '0' && c <= '9')
          val += c - '0';
        else if (c >= 'a' && c <= 'f')
          val += 10 + c - 'a';
        else if (c >= 'A' && c <= 'F')
          val += 10 + c - 'A';
      }
      if (val > 0 && val < 256) {
        char ch = (char)val;
        decoded = decoded + String(&ch, 1);
      }
      i += 2;
    } else if (p[i] == '+') {
      decoded = decoded + " ";
    } else {
      decoded = decoded + String(&p[i], 1);
    }
  }

  // Normalize slashes to forward slash
  decoded = decoded.replace("\\", "/");

  // Remove query string if present
  int queryPos = decoded.getPositionOf("?");
  if (queryPos >= 0) {
    decoded = decoded.substring(0, queryPos);
  }

  // Collapse multiple slashes
  while (decoded.contains("//")) {
    decoded = decoded.replace("//", "/");
  }

  return decoded;
}

static bool IsPathSafe(const String &requestPath) {
  // Check for path traversal attempts
  if (requestPath.contains(".."))
    return false;
  if (requestPath.contains("./"))
    return false;
  if (requestPath.contains("/."))
    return false;

  // Check for null bytes
  const char *p = requestPath.c_str();
  int len = requestPath.byteLength();
  for (int i = 0; i < len; ++i) {
    if (p[i] == '\0')
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
// Directory Listing
//------------------------------------------------------------------------------

static String GenerateDirectoryListing(const Path &dir, const String &urlPath) {
  String html = String(
      "<!DOCTYPE html>\n"
      "<html><head><title>Index of ",
      urlPath,
      "</title>\n"
      "<style>\n"
      "body{font-family:system-ui,sans-serif;max-width:900px;margin:40px "
      "auto;padding:0 20px}\n"
      "h1{color:#333;border-bottom:1px solid #ccc;padding-bottom:10px}\n"
      "table{width:100%;border-collapse:collapse}\n"
      "th,td{text-align:left;padding:8px 12px;border-bottom:1px solid #eee}\n"
      "th{background:#f5f5f5;font-weight:600}\n"
      "a{color:#0066cc;text-decoration:none}\n"
      "a:hover{text-decoration:underline}\n"
      ".dir{font-weight:500}\n"
      ".size{color:#666;text-align:right}\n"
      "</style></head>\n"
      "<body><h1>Index of ",
      urlPath,
      "</h1>\n"
      "<table><thead><tr><th>Name</th><th "
      "class=\"size\">Size</th></tr></thead>\n"
      "<tbody>\n");

  // Add parent directory link if not at root
  if (urlPath != "/") {
    String parentUrl = urlPath;
    if (parentUrl.endsWith("/")) {
      parentUrl = parentUrl.substring(0, parentUrl.length() - 1);
    }
    int lastSlash = parentUrl.getPositionOf("/");
    // Find the actual last slash
    int pos = 0;
    while (true) {
      int next = parentUrl.getPositionOf("/", pos + 1);
      if (next < 0)
        break;
      lastSlash = next;
      pos = next;
    }
    if (lastSlash > 0) {
      parentUrl = parentUrl.substring(0, lastSlash + 1);
    } else {
      parentUrl = "/";
    }
    html = html + "<tr><td class=\"dir\"><a href=\"" + parentUrl +
           "\">../</a></td>"
           "<td class=\"size\">-</td></tr>\n";
  }

  // List children
  List children = dir.listChildren(false);

  // Separate directories and files, then sort each
  List dirs;
  List files;

  for (int i = 0; i < children.length(); ++i) {
    String name = children.at<String>(i);
    Path child(dir.toString() + "/" + name);
    if (child.isDirectory()) {
      dirs.append(name);
    } else {
      files.append(name);
    }
  }

  dirs.sort();
  files.sort();

  // Output directories first
  for (int i = 0; i < dirs.length(); ++i) {
    String name = dirs.at<String>(i);
    String href = urlPath;
    if (!href.endsWith("/"))
      href = href + "/";
    href = href + name + "/";

    html = html + "<tr><td class=\"dir\"><a href=\"" + href + "\">" + name +
           "/</a></td>"
           "<td class=\"size\">-</td></tr>\n";
  }

  // Then files
  for (int i = 0; i < files.length(); ++i) {
    String name = files.at<String>(i);
    Path filePath(dir.toString() + "/" + name);
    long long size = filePath.getSize();

    String sizeStr;
    if (size < 1024) {
      sizeStr = String((int)size, " B");
    } else if (size < 1024 * 1024) {
      sizeStr = String((int)(size / 1024), " KB");
    } else if (size < 1024LL * 1024 * 1024) {
      sizeStr = String((int)(size / (1024 * 1024)), " MB");
    } else {
      sizeStr = String((int)(size / (1024LL * 1024 * 1024)), " GB");
    }

    String href = urlPath;
    if (!href.endsWith("/"))
      href = href + "/";
    href = href + name;

    html = html + "<tr><td><a href=\"" + href + "\">" + name +
           "</a></td>"
           "<td class=\"size\">" +
           sizeStr + "</td></tr>\n";
  }

  html = html + "</tbody></table>\n"
                "<hr><p><em>attoboy-httpd</em></p></body></html>\n";

  return html;
}

//------------------------------------------------------------------------------
// Request Parsing
//------------------------------------------------------------------------------

struct HttpRequest {
  String method;
  String path;
  bool valid;
};

static HttpRequest ParseRequest(const String &raw) {
  HttpRequest req;
  req.valid = false;

  // Find the first line
  int lineEnd = raw.getPositionOf("\r\n");
  if (lineEnd < 0) {
    lineEnd = raw.getPositionOf("\n");
  }
  if (lineEnd < 0)
    return req;

  String requestLine = raw.substring(0, lineEnd);
  List parts = requestLine.split(" ", 3);

  if (parts.length() < 2)
    return req;

  req.method = parts.at<String>(0).upper();
  req.path = parts.at<String>(1);
  req.valid = true;

  return req;
}

//------------------------------------------------------------------------------
// Request Handler
//------------------------------------------------------------------------------

static void HandleClient(File &client, const Path &rootDir) {
  // Read the request (simple approach: read what's available)
  String request;

  // Wait briefly for data
  Sleep(10);

  // Read available data
  if (client.hasAvailable()) {
    request = client.readToString(8192);
  }

  if (request.isEmpty()) {
    client.close();
    return;
  }

  // Parse the request
  HttpRequest req = ParseRequest(request);

  if (!req.valid) {
    Log(req.path, " -> 400");
    SendErrorResponse(client, 400);
    client.close();
    return;
  }

  // Only allow GET
  if (req.method != "GET") {
    Log(req.path, " -> 405");
    SendErrorResponse(client, 405);
    client.close();
    return;
  }

  // Normalize and validate path
  String urlPath = NormalizePath(req.path);

  if (!IsPathSafe(urlPath)) {
    Log(urlPath, " -> 403");
    SendErrorResponse(client, 403);
    client.close();
    return;
  }

  // Build filesystem path
  String fsPath = rootDir.toString();
  if (urlPath != "/") {
    // Convert URL path to filesystem path
    String relativePath = urlPath;
    if (relativePath.startsWith("/")) {
      relativePath = relativePath.substring(1);
    }
    // Use backslash for Windows
    relativePath = relativePath.replace("/", "\\");
    fsPath = fsPath + "\\" + relativePath;
  }

  Path targetPath(fsPath);

  // Check if path exists
  if (!targetPath.exists()) {
    Log(urlPath, " -> 404");
    SendErrorResponse(client, 404);
    client.close();
    return;
  }

  // Security: verify the resolved path is within root
  if (!targetPath.isWithin(rootDir) && !targetPath.equals(rootDir)) {
    Log(urlPath, " -> 403");
    SendErrorResponse(client, 403);
    client.close();
    return;
  }

  // Handle directory
  if (targetPath.isDirectory()) {
    // Check for index.html
    Path indexPath(targetPath.toString() + "\\index.html");
    if (indexPath.exists() && indexPath.isRegularFile()) {
      targetPath = indexPath;
    } else {
      // Generate directory listing
      String listing = GenerateDirectoryListing(targetPath, urlPath);
      String headers = BuildResponseHeaders(200, "text/html; charset=utf-8",
                                            listing.byteLength());
      client.write(headers);
      client.write(listing);
      Log(urlPath, " -> 200 (directory)");
      client.close();
      return;
    }
  }

  // Serve file
  if (!targetPath.isRegularFile()) {
    Log(urlPath, " -> 404");
    SendErrorResponse(client, 404);
    client.close();
    return;
  }

  // Read file content
  Buffer content = targetPath.readToBuffer();
  String contentType = GetContentType(targetPath.getExtension());

  // Send response
  String headers = BuildResponseHeaders(200, contentType, content.length());
  client.write(headers);

  // Write content in chunks for large files
  int len = 0;
  const unsigned char *data = content.c_ptr(&len);
  int written = 0;
  while (written < len) {
    int chunk = len - written;
    if (chunk > 65536)
      chunk = 65536;
    int n = client.writeUpTo(Buffer(data + written, chunk));
    if (n <= 0)
      break;
    written += n;
  }

  Log(urlPath, " -> 200");
  client.close();
}

//------------------------------------------------------------------------------
// Entry Point
//------------------------------------------------------------------------------

extern "C" void atto_main() {
  Arguments args;

  args.addParameter("p", "Port to listen on", "8123", "port");
  args.addPositionalParameter(
      "path", "Directory to serve (default: current directory)");
  args.setHelp(
      "Attoboy HTTP File Server\n\n"
      "A simple single-threaded HTTP server for serving static files.\n\n"
      "Usage: attoboy_httpd [options] [path]\n\n"
      "Examples:\n"
      "  attoboy_httpd                    Serve current directory on port "
      "8123\n"
      "  attoboy_httpd -p 8080            Serve current directory on port "
      "8080\n"
      "  attoboy_httpd C:\\www             Serve C:\\www on port 8123\n"
      "  attoboy_httpd --port=80 D:\\site  Serve D:\\site on port 80\n");

  Map parsed = args.parseArguments();

  // Get port
  int port = parsed.get<String, String>("p", "8123").toInteger();
  if (port <= 0 || port > 65535) {
    LogError("Invalid port number. Must be between 1 and 65535.");
    Exit(1);
  }

  // Get root directory
  String rootPath = parsed.get<String, String>("path", ".");
  if (rootPath.isEmpty()) {
    rootPath = ".";
  }

  Path rootDir(rootPath);

  if (!rootDir.exists()) {
    LogError("Directory does not exist: ", rootPath);
    Exit(1);
  }

  if (!rootDir.isDirectory()) {
    LogError("Path is not a directory: ", rootPath);
    Exit(1);
  }

  // Create server socket
  File server(port);

  if (!server.isValid()) {
    LogError("Failed to create server socket on port ", port);
    LogError("The port may be in use or require administrator privileges.");
    Exit(1);
  }

  Log("===========================================");
  Log("  Attoboy HTTP File Server");
  Log("===========================================");
  Log("Serving:  ", rootDir.toString());
  Log("Address:  http://localhost:", port, "/");
  Log("-------------------------------------------");
  Log("Press Ctrl+C to stop the server.");
  Log("");

  // Main server loop
  while (true) {
    File client = server.accept();

    if (client.isValid()) {
      HandleClient(client, rootDir);
    }
  }
}