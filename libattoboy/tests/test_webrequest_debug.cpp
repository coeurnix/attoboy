#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  WebRequest req(String("http://httpbin.org/get"));
  const WebResponse *resp = req.doGet(30000);
  if (resp) {
    delete resp;
  }
  Exit(0);
}
