#include "ir.hpp"
#include "json.hpp"

using namespace nlohmann;

Function fn2ir(json &fn) { return Function(fn); }

vector<Function> json2ir(json &info) {
  auto fns = info["functions"];
  vector<Function> functions;
  for (auto fn : fns) {
    functions.push_back(fn2ir(fn));
  }
  return functions;
}
