#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  WebRequest req(String("http://httpbin.org/get"));
  WebResponse resp = req.doGet(30000);
  if (resp.succeeded()) {
  }
  Exit(0);
}
