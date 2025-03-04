#include "ir_gen/json_translate.hpp"
#include "test_pass.hpp"
#include <iostream>

using namespace std;

int main() {
  string input;
  string line;
  while (std::getline(std::cin, line)) {
    input += line;
  }
  auto pass_manager = JsonTranslator::json2pm(input);
  TestPass lp = TestPass();
  pass_manager.pass(lp);
}
