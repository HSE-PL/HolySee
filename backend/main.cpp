#include "ir/ir.hpp"
#include "test_pass.hpp"
#include "json/json.hpp"
#include "json/json2ir.hpp"
#include <cstdio>
#include <iostream>

using namespace std;
using namespace nlohmann;

int main() {
  string input;
  string line;
  while (std::getline(std::cin, line)) {
    input += line;
  }
  json inp_json = json::parse(input);
  auto pass_manager = json2ir(inp_json);
  TestPass lp = TestPass();
  pass_manager.pass(lp);
  /*cout << program;*/
}
