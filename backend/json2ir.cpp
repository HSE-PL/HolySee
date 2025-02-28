#include "ir.hpp"
#include "json.hpp"
#include <iostream>

using namespace std;

using namespace nlohmann;

Function fn2ir(json &fn) { return Function(fn); }

vector<Function> json2ir(json &info) {
  auto fns = info["functions"];
  vector<Function> functions;
  for (auto fn : fns) {
    auto func = fn2ir(fn);
    functions.push_back(func);
    cout << func;
  }
  return functions;
}
