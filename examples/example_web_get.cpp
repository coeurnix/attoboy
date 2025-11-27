//==============================================================================
// example_web_get.cpp - Simple URL Downloader
//==============================================================================
// Downloads a URL to a file, or prints its contents to the console.
//
// Usage:
//   example_web_get <url>                  - Print content to console
//   example_web_get <url> <output_file>    - Save content to file
//
// Example:
//   example_web_get https://example.com/data.json
//   example_web_get https://example.com/file.zip download.zip
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

extern "C" void atto_main() {
  // Set up command-line arguments
  Arguments args;
  args.addPositionalParameter("url", "URL to download")
      .addPositionalParameter("output",
                              "Output file (prints to console if omitted)")
      .setHelp("example_web_get - Simple URL Downloader\n"
               "Downloads a URL to a file or prints to console.\n\n"
               "Usage: example_web_get <url> [output_file]");

  args.requireArgument("url");

  // Parse arguments (returns empty Map if -h/--help or error)
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    return;
  }

  // Get the URL (required)
  String url = parsed.get<String, String>("url");
  if (url.isEmpty()) {
    LogError("No URL provided. Use -h for help.");
    return;
  }

  // Get optional output path
  String outputPath = parsed.get<String, String>("output");

  if (!outputPath.isEmpty()) {
    // Download directly to file using the convenient static method
    Log("Downloading: ", url);

    if (WebRequest::Download(url, outputPath)) {
      Log("Saved to: ", outputPath);
    } else {
      LogError("Download failed: ", url);
      Exit(1);
    }
  } else {
    // Fetch and print to console
    WebRequest request(url);
    WebResponse response = request.doGet();

    if (response.succeeded()) {
      // Print the response body
      Log(response.asString());
    } else {
      LogError("Request failed: ", response.getStatusCode(), " ",
               response.getStatusReason());
      Exit(1);
    }
  }

  Exit(0);
}