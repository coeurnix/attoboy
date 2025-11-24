#include "attoboy/attoboy.h"

using namespace attoboy;

void atto_main() {
  Log(ATTO_TEXT("=== Testing JSON Output for Nested Collections ===\n"));

  // Test 1: Simple nested List
  Log(ATTO_TEXT("Test 1: Simple nested List"));
  List inner1;
  inner1.append(1).append(2).append(3);
  List inner2;
  inner2.append(4).append(5).append(6);
  List outer1;
  outer1.append(inner1).append(inner2);

  String json1(outer1);
  Log(ATTO_TEXT("  Expected: [[1,2,3],[4,5,6]]"));
  Log(ATTO_TEXT("  Actual:   "), json1);
  Log(ATTO_TEXT(""));

  // Test 2: Triple nested List
  Log(ATTO_TEXT("Test 2: Triple nested List"));
  List l3a;
  l3a.append(1).append(2);
  List l2a;
  l2a.append(l3a);
  List l1;
  l1.append(l2a);

  String json2(l1);
  Log(ATTO_TEXT("  Expected: [[[1,2]]]"));
  Log(ATTO_TEXT("  Actual:   "), json2);
  Log(ATTO_TEXT(""));

  // Test 3: Nested Map
  Log(ATTO_TEXT("Test 3: Nested Map"));
  Map innerMap;
  innerMap.put(ATTO_TEXT("x"), 10).put(ATTO_TEXT("y"), 20);
  Map outerMap;
  outerMap.put(ATTO_TEXT("coords"), innerMap);

  String json3(outerMap);
  Log(ATTO_TEXT("  Expected: {\"coords\":{\"x\":10,\"y\":20}}"));
  Log(ATTO_TEXT("  Actual:   "), json3);
  Log(ATTO_TEXT(""));

  // Test 4: Map with List
  Log(ATTO_TEXT("Test 4: Map with List"));
  List items;
  items.append(ATTO_TEXT("a")).append(ATTO_TEXT("b")).append(ATTO_TEXT("c"));
  Map mapWithList;
  mapWithList.put(ATTO_TEXT("items"), items);

  String json4(mapWithList);
  Log(ATTO_TEXT("  Expected: {\"items\":[\"a\",\"b\",\"c\"]}"));
  Log(ATTO_TEXT("  Actual:   "), json4);
  Log(ATTO_TEXT(""));

  // Test 5: List with Map
  Log(ATTO_TEXT("Test 5: List with Map"));
  Map user;
  user.put(ATTO_TEXT("id"), 1).put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
  List users;
  users.append(user);

  String json5(users);
  Log(ATTO_TEXT("  Expected: [{\"id\":1,\"name\":\"Alice\"}]"));
  Log(ATTO_TEXT("  Actual:   "), json5);
  Log(ATTO_TEXT(""));

  // Test 6: Mixed types in List
  Log(ATTO_TEXT("Test 6: Mixed types in List"));
  List mixedInner;
  mixedInner.append(10).append(20);
  List mixed;
  mixed.append(ATTO_TEXT("string"));
  mixed.append(42);
  mixed.append(3.14f);
  mixed.append(true);
  mixed.append(mixedInner);

  String json6(mixed);
  Log(ATTO_TEXT("  Expected: [\"string\",42,3.14,true,[10,20]]"));
  Log(ATTO_TEXT("  Actual:   "), json6);
  Log(ATTO_TEXT(""));

  // Test 7: Complex realistic structure
  Log(ATTO_TEXT("Test 7: Complex realistic structure"));
  Map user1;
  user1.put(ATTO_TEXT("id"), 1);
  user1.put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
  List tags1;
  tags1.append(ATTO_TEXT("admin")).append(ATTO_TEXT("dev"));
  user1.put(ATTO_TEXT("tags"), tags1);

  Map user2;
  user2.put(ATTO_TEXT("id"), 2);
  user2.put(ATTO_TEXT("name"), ATTO_TEXT("Bob"));
  List tags2;
  tags2.append(ATTO_TEXT("user"));
  user2.put(ATTO_TEXT("tags"), tags2);

  List allUsers;
  allUsers.append(user1).append(user2);

  Map database;
  database.put(ATTO_TEXT("users"), allUsers);
  database.put(ATTO_TEXT("version"), 1);

  String json7(database);
  Log(ATTO_TEXT("  Database JSON:"));
  Log(ATTO_TEXT("  "), json7);
  Log(ATTO_TEXT(""));

  // Test 8: Set to JSON (converts to array)
  Log(ATTO_TEXT("Test 8: Set to JSON"));
  Set s;
  s.put(3).put(1).put(2);

  String json8(s);
  Log(ATTO_TEXT("  Expected: Array of values (order may vary)"));
  Log(ATTO_TEXT("  Actual:   "), json8);
  Log(ATTO_TEXT(""));

  // Test 9: Empty nested collections
  Log(ATTO_TEXT("Test 9: Empty nested collections"));
  List emptyInner;
  List emptyOuter;
  emptyOuter.append(emptyInner);

  String json9(emptyOuter);
  Log(ATTO_TEXT("  Expected: [[]]"));
  Log(ATTO_TEXT("  Actual:   "), json9);
  Log(ATTO_TEXT(""));

  Map emptyInnerMap;
  Map emptyOuterMap;
  emptyOuterMap.put(ATTO_TEXT("empty"), emptyInnerMap);

  String json10(emptyOuterMap);
  Log(ATTO_TEXT("  Expected: {\"empty\":{}}"));
  Log(ATTO_TEXT("  Actual:   "), json10);
  Log(ATTO_TEXT(""));

  // Test 10: Triple nested Map
  Log(ATTO_TEXT("Test 10: Triple nested Map"));
  Map deep3;
  deep3.put(ATTO_TEXT("value"), 42);
  Map deep2;
  deep2.put(ATTO_TEXT("level3"), deep3);
  Map deep1;
  deep1.put(ATTO_TEXT("level2"), deep2);

  String json11(deep1);
  Log(ATTO_TEXT("  Expected: {\"level2\":{\"level3\":{\"value\":42}}}"));
  Log(ATTO_TEXT("  Actual:   "), json11);
  Log(ATTO_TEXT(""));

  Log(ATTO_TEXT("=== All JSON output tests completed successfully ==="));
  Exit(0);
}
