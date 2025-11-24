#include "attoboy/attoboy.h"

using namespace attoboy;

static int errorCount = 0;

#define TEST(condition, message)                                               \
  if (!(condition)) {                                                          \
    LogError(ATTO_TEXT("Test failed: "), message);                            \
    errorCount++;                                                              \
  }

void atto_main() {
  Log(ATTO_TEXT("Running nested collections tests..."));

  // ============================================================================
  // Level 1: Basic nested collections (already tested in individual tests)
  // ============================================================================

  // List containing Lists
  List innerList1;
  innerList1.append(1).append(2).append(3);
  List innerList2;
  innerList2.append(4).append(5).append(6);
  List listOfLists;
  listOfLists.append(innerList1).append(innerList2);

  TEST(listOfLists.length() == 2, ATTO_TEXT("List of Lists - length"));
  TEST(listOfLists.typeAt(0) == TYPE_LIST, ATTO_TEXT("List of Lists - type check"));
  List retrieved1 = listOfLists.at<List>(0);
  TEST(retrieved1.length() == 3, ATTO_TEXT("List of Lists - inner length"));
  TEST(retrieved1.at<int>(0) == 1, ATTO_TEXT("List of Lists - inner value"));

  // Map containing Lists
  List valuesList;
  valuesList.append(ATTO_TEXT("a")).append(ATTO_TEXT("b")).append(ATTO_TEXT("c"));
  Map mapWithList;
  mapWithList.put(ATTO_TEXT("items"), valuesList);

  TEST(mapWithList.length() == 1, ATTO_TEXT("Map with List - length"));
  List retrievedList = mapWithList.get<String, List>(ATTO_TEXT("items"));
  TEST(retrievedList.length() == 3, ATTO_TEXT("Map with List - inner length"));
  TEST(retrievedList.at<String>(0) == ATTO_TEXT("a"),
       ATTO_TEXT("Map with List - inner value"));

  // Map containing Maps
  Map innerMap1;
  innerMap1.put(ATTO_TEXT("x"), 10).put(ATTO_TEXT("y"), 20);
  Map outerMap;
  outerMap.put(ATTO_TEXT("inner"), innerMap1);

  TEST(outerMap.length() == 1, ATTO_TEXT("Map with Map - length"));
  Map retrievedMap = outerMap.get<String, Map>(ATTO_TEXT("inner"));
  TEST(retrievedMap.length() == 2, ATTO_TEXT("Map with Map - inner length"));
  TEST((retrievedMap.get<String, int>(ATTO_TEXT("x")) == 10), ATTO_TEXT("Map with Map - inner value"));

  // List containing Maps
  Map map1;
  map1.put(ATTO_TEXT("id"), 1).put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
  Map map2;
  map2.put(ATTO_TEXT("id"), 2).put(ATTO_TEXT("name"), ATTO_TEXT("Bob"));
  List listOfMaps;
  listOfMaps.append(map1).append(map2);

  TEST(listOfMaps.length() == 2, ATTO_TEXT("List of Maps - length"));
  Map retrievedMap1 = listOfMaps.at<Map>(0);
  TEST((retrievedMap1.get<String, int>(ATTO_TEXT("id")) == 1), ATTO_TEXT("List of Maps - value"));

  // Set containing Lists (via List since Set stores values)
  List setList1;
  setList1.append(1).append(2);
  List setList2;
  setList2.append(3).append(4);
  Set setOfLists;
  setOfLists.put(setList1).put(setList2);
  TEST(setOfLists.length() == 2, ATTO_TEXT("Set of Lists - length"));

  // Set containing Maps
  Map setMap1;
  setMap1.put(ATTO_TEXT("a"), 1);
  Map setMap2;
  setMap2.put(ATTO_TEXT("b"), 2);
  Set setOfMaps;
  setOfMaps.put(setMap1).put(setMap2);
  TEST(setOfMaps.length() == 2, ATTO_TEXT("Set of Maps - length"));

  Log(ATTO_TEXT("Level 1 nested tests passed"));

  // ============================================================================
  // Level 2: Nested collections (List -> List -> primitives)
  // ============================================================================

  // List -> List -> List
  List level3_1;
  level3_1.append(1).append(2);
  List level3_2;
  level3_2.append(3).append(4);

  List level2_1;
  level2_1.append(level3_1).append(level3_2);

  List level3_3;
  level3_3.append(5).append(6);
  List level3_4;
  level3_4.append(7).append(8);

  List level2_2;
  level2_2.append(level3_3).append(level3_4);

  List level1;
  level1.append(level2_1).append(level2_2);

  TEST(level1.length() == 2, ATTO_TEXT("Triple nested List - L1 length"));
  List l2 = level1.at<List>(0);
  TEST(l2.length() == 2, ATTO_TEXT("Triple nested List - L2 length"));
  List l3 = l2.at<List>(0);
  TEST(l3.length() == 2, ATTO_TEXT("Triple nested List - L3 length"));
  TEST(l3.at<int>(0) == 1, ATTO_TEXT("Triple nested List - L3 value"));

  // Map -> Map -> Map
  Map deepMap3;
  deepMap3.put(ATTO_TEXT("value"), 42);

  Map deepMap2;
  deepMap2.put(ATTO_TEXT("deep"), deepMap3);

  Map deepMap1;
  deepMap1.put(ATTO_TEXT("nested"), deepMap2);

  TEST(deepMap1.length() == 1, ATTO_TEXT("Triple nested Map - L1 length"));
  Map dm2 = deepMap1.get<String, Map>(ATTO_TEXT("nested"));
  TEST(dm2.length() == 1, ATTO_TEXT("Triple nested Map - L2 length"));
  Map dm3 = dm2.get<String, Map>(ATTO_TEXT("deep"));
  TEST(dm3.length() == 1, ATTO_TEXT("Triple nested Map - L3 length"));
  TEST((dm3.get<String, int>(ATTO_TEXT("value")) == 42), ATTO_TEXT("Triple nested Map - L3 value"));

  // Map -> List -> Map
  Map complexInner;
  complexInner.put(ATTO_TEXT("id"), 100);

  List complexMid;
  complexMid.append(complexInner);

  Map complexOuter;
  complexOuter.put(ATTO_TEXT("items"), complexMid);

  TEST(complexOuter.length() == 1, ATTO_TEXT("Map->List->Map - L1 length"));
  List cmid = complexOuter.get<String, List>(ATTO_TEXT("items"));
  TEST(cmid.length() == 1, ATTO_TEXT("Map->List->Map - L2 length"));
  Map cinner = cmid.at<Map>(0);
  TEST((cinner.get<String, int>(ATTO_TEXT("id")) == 100), ATTO_TEXT("Map->List->Map - value"));

  // List -> Map -> List
  List innerMost;
  innerMost.append(ATTO_TEXT("x")).append(ATTO_TEXT("y")).append(ATTO_TEXT("z"));

  Map middleMap;
  middleMap.put(ATTO_TEXT("coords"), innerMost);

  List outerList;
  outerList.append(middleMap);

  TEST(outerList.length() == 1, ATTO_TEXT("List->Map->List - L1 length"));
  Map mmap = outerList.at<Map>(0);
  TEST(mmap.length() == 1, ATTO_TEXT("List->Map->List - L2 length"));
  List llist = mmap.get<String, List>(ATTO_TEXT("coords"));
  TEST(llist.length() == 3, ATTO_TEXT("List->Map->List - L3 length"));
  TEST(llist.at<String>(0) == ATTO_TEXT("x"),
       ATTO_TEXT("List->Map->List - value"));

  Log(ATTO_TEXT("Level 2 nested tests passed"));

  // ============================================================================
  // Level 3: Mixed type collections
  // ============================================================================

  // List with mixed nested types
  List mixedInner1;
  mixedInner1.append(1).append(2).append(3);

  Map mixedInner2;
  mixedInner2.put(ATTO_TEXT("key"), ATTO_TEXT("value"));

  List mixedOuter;
  mixedOuter.append(ATTO_TEXT("string"));
  mixedOuter.append(42);
  mixedOuter.append(3.14f);
  mixedOuter.append(mixedInner1);
  mixedOuter.append(mixedInner2);

  TEST(mixedOuter.length() == 5, ATTO_TEXT("Mixed List - length"));
  TEST(mixedOuter.typeAt(0) == TYPE_STRING, ATTO_TEXT("Mixed List - type 0"));
  TEST(mixedOuter.typeAt(1) == TYPE_INT, ATTO_TEXT("Mixed List - type 1"));
  TEST(mixedOuter.typeAt(2) == TYPE_FLOAT, ATTO_TEXT("Mixed List - type 2"));
  TEST(mixedOuter.typeAt(3) == TYPE_LIST, ATTO_TEXT("Mixed List - type 3"));
  TEST(mixedOuter.typeAt(4) == TYPE_MAP, ATTO_TEXT("Mixed List - type 4"));

  // Map with mixed nested types
  List mapMixedList;
  mapMixedList.append(10).append(20);

  Map mapMixedMap;
  mapMixedMap.put(1, ATTO_TEXT("one"));

  Map mixedMap;
  mixedMap.put(ATTO_TEXT("str"), ATTO_TEXT("hello"));
  mixedMap.put(ATTO_TEXT("num"), 99);
  mixedMap.put(ATTO_TEXT("list"), mapMixedList);
  mixedMap.put(ATTO_TEXT("map"), mapMixedMap);

  TEST(mixedMap.length() == 4, ATTO_TEXT("Mixed Map - length"));
  TEST(mixedMap.typeAt(ATTO_TEXT("str")) == TYPE_STRING,
       ATTO_TEXT("Mixed Map - type str"));
  TEST(mixedMap.typeAt(ATTO_TEXT("num")) == TYPE_INT,
       ATTO_TEXT("Mixed Map - type num"));
  TEST(mixedMap.typeAt(ATTO_TEXT("list")) == TYPE_LIST,
       ATTO_TEXT("Mixed Map - type list"));
  TEST(mixedMap.typeAt(ATTO_TEXT("map")) == TYPE_MAP,
       ATTO_TEXT("Mixed Map - type map"));

  Log(ATTO_TEXT("Level 3 mixed type tests passed"));

  // ============================================================================
  // JSON Output Testing
  // ============================================================================

  // Simple nested List JSON
  List jsonList1;
  jsonList1.append(1).append(2);
  List jsonList2;
  jsonList2.append(3).append(4);
  List jsonOuter;
  jsonOuter.append(jsonList1).append(jsonList2);

  String jsonListStr(jsonOuter);
  TEST(jsonListStr.contains(ATTO_TEXT("[")),
       ATTO_TEXT("Nested List JSON - contains ["));
  TEST(jsonListStr.contains(ATTO_TEXT("]")),
       ATTO_TEXT("Nested List JSON - contains ]"));
  TEST(jsonListStr.contains(ATTO_TEXT("1")),
       ATTO_TEXT("Nested List JSON - contains value"));

  // Simple nested Map JSON
  Map jsonInnerMap;
  jsonInnerMap.put(ATTO_TEXT("x"), 10);
  Map jsonOuterMap;
  jsonOuterMap.put(ATTO_TEXT("inner"), jsonInnerMap);

  String jsonMapStr(jsonOuterMap);
  TEST(jsonMapStr.contains(ATTO_TEXT("{")),
       ATTO_TEXT("Nested Map JSON - contains {"));
  TEST(jsonMapStr.contains(ATTO_TEXT("}")),
       ATTO_TEXT("Nested Map JSON - contains }"));
  TEST(jsonMapStr.contains(ATTO_TEXT("inner")),
       ATTO_TEXT("Nested Map JSON - contains key"));
  TEST(jsonMapStr.contains(ATTO_TEXT("x")),
       ATTO_TEXT("Nested Map JSON - contains inner key"));
  TEST(jsonMapStr.contains(ATTO_TEXT("10")),
       ATTO_TEXT("Nested Map JSON - contains value"));

  // Complex nested structure JSON
  List complexArray;
  complexArray.append(1).append(ATTO_TEXT("two")).append(3.0f);

  Map complexData;
  complexData.put(ATTO_TEXT("array"), complexArray);
  complexData.put(ATTO_TEXT("flag"), true);
  complexData.put(ATTO_TEXT("count"), 5);

  List topLevel;
  topLevel.append(complexData);

  String complexJson(topLevel);
  TEST(complexJson.contains(ATTO_TEXT("array")),
       ATTO_TEXT("Complex JSON - contains array key"));
  TEST(complexJson.contains(ATTO_TEXT("flag")),
       ATTO_TEXT("Complex JSON - contains flag key"));
  TEST(complexJson.contains(ATTO_TEXT("true")),
       ATTO_TEXT("Complex JSON - contains bool value"));
  TEST(complexJson.contains(ATTO_TEXT("two")),
       ATTO_TEXT("Complex JSON - contains string value"));

  // Triple nested JSON
  List tripleInner;
  tripleInner.append(ATTO_TEXT("deep"));

  List tripleMid;
  tripleMid.append(tripleInner);

  List tripleOuter;
  tripleOuter.append(tripleMid);

  String tripleJson(tripleOuter);
  TEST(tripleJson.contains(ATTO_TEXT("deep")),
       ATTO_TEXT("Triple nested JSON - contains deep value"));
  // Count the bracket nesting - should have [[[
  int bracketCount = 0;
  for (int i = 0; i < tripleJson.length() - 2; i++) {
    if (tripleJson.at(i) == ATTO_TEXT("[") &&
        tripleJson.at(i + 1) == ATTO_TEXT("[") &&
        tripleJson.at(i + 2) == ATTO_TEXT("[")) {
      bracketCount++;
      break;
    }
  }
  TEST(bracketCount == 1, ATTO_TEXT("Triple nested JSON - has [[[ pattern"));

  // Map with nested Map JSON - verify structure
  Map jsonNestedMap1;
  jsonNestedMap1.put(ATTO_TEXT("a"), 1).put(ATTO_TEXT("b"), 2);

  Map jsonNestedMap0;
  jsonNestedMap0.put(ATTO_TEXT("nested"), jsonNestedMap1);

  String nestedMapJson(jsonNestedMap0);
  TEST(nestedMapJson.contains(ATTO_TEXT("\"nested\":")),
       ATTO_TEXT("Map JSON - contains key:value separator"));
  TEST(nestedMapJson.contains(ATTO_TEXT("\"a\":")),
       ATTO_TEXT("Map JSON - contains inner key"));

  // Set JSON output (converted to List)
  Set jsonSet;
  jsonSet.put(1).put(2).put(3);
  String setJson(jsonSet);
  TEST(setJson.contains(ATTO_TEXT("[")),
       ATTO_TEXT("Set JSON - converted to array"));
  TEST(setJson.contains(ATTO_TEXT("1")),
       ATTO_TEXT("Set JSON - contains value"));

  Log(ATTO_TEXT("JSON output tests passed"));

  // ============================================================================
  // Edge Cases
  // ============================================================================

  // Empty nested collections
  List emptyInner;
  List emptyOuter;
  emptyOuter.append(emptyInner);
  TEST(emptyOuter.length() == 1, ATTO_TEXT("Empty nested List - outer length"));
  List emptyRetrieved = emptyOuter.at<List>(0);
  TEST(emptyRetrieved.length() == 0,
       ATTO_TEXT("Empty nested List - inner length"));

  Map emptyInnerMap;
  Map emptyOuterMap;
  emptyOuterMap.put(ATTO_TEXT("empty"), emptyInnerMap);
  TEST(emptyOuterMap.length() == 1,
       ATTO_TEXT("Empty nested Map - outer length"));
  Map emptyMapRetrieved = emptyOuterMap.get<String, Map>(ATTO_TEXT("empty"));
  TEST(emptyMapRetrieved.length() == 0,
       ATTO_TEXT("Empty nested Map - inner length"));

  // Empty collections JSON
  List emptyListForJson;
  emptyListForJson.append(List());
  String emptyListJson(emptyListForJson);
  TEST(emptyListJson.contains(ATTO_TEXT("[]")),
       ATTO_TEXT("Empty nested List JSON - contains []"));

  Map emptyMapForJson;
  emptyMapForJson.put(ATTO_TEXT("e"), Map());
  String emptyMapJson(emptyMapForJson);
  TEST(emptyMapJson.contains(ATTO_TEXT("{}")),
       ATTO_TEXT("Empty nested Map JSON - contains {}"));

  Log(ATTO_TEXT("Edge case tests passed"));

  // ============================================================================
  // Verify copy/duplicate with nested collections
  // ============================================================================

  // Copy List with nested Lists
  List copySource;
  List copyInner;
  copyInner.append(100).append(200);
  copySource.append(copyInner);

  List copyDest = copySource.duplicate();
  TEST(copyDest.length() == 1, ATTO_TEXT("Copy nested List - length"));
  List copyInnerRetrieved = copyDest.at<List>(0);
  TEST(copyInnerRetrieved.length() == 2,
       ATTO_TEXT("Copy nested List - inner length"));
  TEST(copyInnerRetrieved.at<int>(0) == 100,
       ATTO_TEXT("Copy nested List - inner value"));

  // Copy Map with nested Maps
  Map copyMapInner;
  copyMapInner.put(ATTO_TEXT("val"), 999);
  Map copyMapSource;
  copyMapSource.put(ATTO_TEXT("data"), copyMapInner);

  Map copyMapDest = copyMapSource.duplicate();
  TEST(copyMapDest.length() == 1, ATTO_TEXT("Copy nested Map - length"));
  Map copyMapInnerRetrieved = copyMapDest.get<String, Map>(ATTO_TEXT("data"));
  TEST(copyMapInnerRetrieved.length() == 1, ATTO_TEXT("Copy nested Map - inner length"));
  TEST((copyMapInnerRetrieved.get<String, int>(ATTO_TEXT("val")) == 999), ATTO_TEXT("Copy nested Map - inner value"));

  Log(ATTO_TEXT("Copy/duplicate tests passed"));

  // ============================================================================
  // Realistic nested structure test
  // ============================================================================

  // Simulate a user database structure
  Map user1;
  user1.put(ATTO_TEXT("id"), 1);
  user1.put(ATTO_TEXT("name"), ATTO_TEXT("Alice"));
  List user1Tags;
  user1Tags.append(ATTO_TEXT("admin")).append(ATTO_TEXT("developer"));
  user1.put(ATTO_TEXT("tags"), user1Tags);

  Map user2;
  user2.put(ATTO_TEXT("id"), 2);
  user2.put(ATTO_TEXT("name"), ATTO_TEXT("Bob"));
  List user2Tags;
  user2Tags.append(ATTO_TEXT("user"));
  user2.put(ATTO_TEXT("tags"), user2Tags);

  List users;
  users.append(user1).append(user2);

  Map database;
  database.put(ATTO_TEXT("users"), users);
  database.put(ATTO_TEXT("version"), 1);

  TEST(database.length() == 2, ATTO_TEXT("Realistic structure - DB length"));
  List dbUsers = database.get<String, List>(ATTO_TEXT("users"));
  TEST(dbUsers.length() == 2, ATTO_TEXT("Realistic structure - users count"));
  Map dbUser1 = dbUsers.at<Map>(0);
  TEST((dbUser1.get<String, String>(ATTO_TEXT("name")) == ATTO_TEXT("Alice")), ATTO_TEXT("Realistic structure - user name"));
  List dbUser1Tags = dbUser1.get<String, List>(ATTO_TEXT("tags"));
  TEST(dbUser1Tags.length() == 2, ATTO_TEXT("Realistic structure - user tags count"));
  TEST((dbUser1Tags.at<String>(0) == ATTO_TEXT("admin")), ATTO_TEXT("Realistic structure - tag value"));

  // JSON output of realistic structure
  String dbJson(database);
  TEST(dbJson.contains(ATTO_TEXT("users")),
       ATTO_TEXT("Realistic JSON - contains users key"));
  TEST(dbJson.contains(ATTO_TEXT("Alice")),
       ATTO_TEXT("Realistic JSON - contains name"));
  TEST(dbJson.contains(ATTO_TEXT("admin")),
       ATTO_TEXT("Realistic JSON - contains tag"));

  Log(ATTO_TEXT("Realistic structure tests passed"));

  // Report results
  if (errorCount == 0) {
    Log(ATTO_TEXT("All nested collections tests passed!"));
    Exit(0);
  } else {
    LogError(errorCount, ATTO_TEXT(" test(s) failed"));
    Exit(errorCount);
  }
}
