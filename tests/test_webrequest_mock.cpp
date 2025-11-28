#include "test_framework.h"

void atto_main() {
  EnableLoggingToConsole();
  Log("=== WebRequest Mock Server Test ===");

  // Test 1: Basic GET request
  {
    Log("Test 1: Basic GET request");
    String url = String("http://localhost:8469/test/hello");
    WebRequest req(url);

    WebResponse resp = req.doGet(5000);
    REGISTER_TESTED(WebRequest_constructor);
    REGISTER_TESTED(WebRequest_doGet);
    REGISTER_TESTED(WebResponse_succeeded);

    if (!resp.succeeded()) {
      LogError("Test 1 FAILED: Request did not succeed");
      LogError("Status code:", resp.getStatusCode());
      return;
    }

    String body = resp.asString();
    REGISTER_TESTED(WebResponse_asString);

    Log("Response:", body);
    ASSERT_TRUE(body.contains(String("Hello")));
    Log("Test 1: PASSED");
  }

  // Test 2: JSON POST request
  {
    Log("Test 2: JSON POST request");
    String url = String("http://localhost:8469/test/echo");

    Map testData;
    testData.put("name", "libattoboy");
    testData.put("version", 1);

    WebRequest req(url);
    WebResponse resp = req.doPost(testData, 5000);
    REGISTER_TESTED(WebRequest_doPost_map);

    if (!resp.succeeded()) {
      LogError("Test 2 FAILED: Request did not succeed");
      return;
    }

    Map jsonResp = resp.asJson();
    REGISTER_TESTED(WebResponse_asJson);

    if (jsonResp.isEmpty()) {
      LogError("Test 2 FAILED: Could not parse JSON");
      return;
    }

    ASSERT_TRUE(jsonResp.hasKey("name"));
    String name = jsonResp.get<String, String>("name");
    ASSERT_TRUE(name.equals(String("libattoboy")));

    Log("Test 2: PASSED");
  }

  // Test 3: Status codes
  {
    Log("Test 3: HTTP status codes");
    String url404 = String("http://localhost:8469/test/status/404");
    WebRequest req404(url404);
    WebResponse resp404 = req404.doGet(5000);

    REGISTER_TESTED(WebResponse_getStatusCode);
    ASSERT_EQ(resp404.getStatusCode(), 404);
    ASSERT_FALSE(resp404.succeeded());

    Log("Test 3: PASSED");
  }

  // Test 4: Response headers
  {
    Log("Test 4: Response headers");
    String url = String("http://localhost:8469/test/hello");
    WebRequest req(url);
    WebResponse resp = req.doGet(5000);

    Map headers = resp.getResponseHeaders();
    REGISTER_TESTED(WebResponse_getResponseHeaders);

    ASSERT_TRUE(headers.hasKey("Content-Type"));
    Log("Test 4: PASSED");
  }

  Log("");
  Log("=== All WebRequest tests passed! ===");
  TestFramework::DisplayCoverage();

  // Clean exit - destructors run automatically
  ExitProcess(0);
}
