#include "ir_gen/json/json_translator.hpp"
#include "test_pass.hpp"
#include <iostream>

using namespace std;

int main() {
  string input;
  string line;
  while (std::getline(std::cin, line)) {
    input += line;
  }
  auto translator = JsonTranslator();
  auto pass_manager = translator.toIR(input);
  /*cout << pass_manager;*/
  translator.toStream(std::cout, pass_manager);
  /*TestPass lp = TestPass();*/
  /*pass_manager.pass(lp);*/
}
