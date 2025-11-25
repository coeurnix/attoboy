#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_webrequest_simple.log", true);
  Log("Testing WebRequest basic construction");

  Log("Creating simple WebRequest");
  WebRequest req(String("https://httpbin.org/get"));
  REGISTER_TESTED(WebRequest_constructor);

  Log("Getting URL");
  String url = req.getUrl();
  REGISTER_TESTED(WebRequest_getUrl);
  Log("URL:", url);

  Log("Test completed successfully");
  TestFramework::WriteCoverageData("test_webrequest_simple");

  Exit(0);
}
