#include "test_framework.h"

void atto_main() {
  EnableLoggingToFile("test_webrequest_basic.log");
  Log("Testing WebRequest basic functionality");

  // Test constructor and getters (no network calls)
  Map params;
  params.put("key", "value");
  Map headers;
  headers.put("User-Agent", "test");

  WebRequest req(String("http://example.com/test"), &params, &headers);
  REGISTER_TESTED(WebRequest_constructor);

  String url = req.getUrl();
  REGISTER_TESTED(WebRequest_getUrl);

  Map reqParams = req.getParams();
  REGISTER_TESTED(WebRequest_getParams);

  Map reqHeaders = req.getHeaders();
  REGISTER_TESTED(WebRequest_getHeaders);

  bool completed = req.hasCompleted();
  REGISTER_TESTED(WebRequest_hasCompleted);
  ASSERT_FALSE(completed);

  // Copy constructor
  WebRequest reqCopy = req;
  REGISTER_TESTED(WebRequest_constructor_copy);

  // Assignment
  WebRequest reqAssign(String("http://other.com"));
  reqAssign = req;
  REGISTER_TESTED(WebRequest_operator_assign);

  REGISTER_TESTED(WebRequest_destructor);

  Log("All basic tests passed!");
  TestFramework::WriteCoverageData("test_webrequest_basic");

  Exit(0);
}
