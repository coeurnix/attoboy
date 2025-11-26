#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  EnableLoggingToFile("test_webrequest_get.log", true);
  Log("Starting GET test");

  WebRequest req(String("http://httpbin.org/get"));
  Log("Request created");

  WebResponse resp = req.doGet(30000);
  Log("Request completed");

  if (resp.succeeded()) {
    Log("Response received");
    int statusCode = resp.getStatusCode();
    Log("Status code:", statusCode);

    String body = resp.asString();
    Log("Body length:", body.byteLength());
    Log("Body:", body);

    Log("Response handled");
  } else {
    Log("No response received");
  }

  Exit(0);
}
