#include "cli.hpp"
#include "../opt/tdce.hpp"
#include "argh.h"

#include "../ir_gen/json/json_translator.hpp"
#include <iostream>

void Runner::run(int argc, char *argv[]) {
  auto cmdl = argh::parser(argc, argv);
  auto opts_enabled = cmdl[{"O1", "opt"}];
  std::string input;
  std::string line;
  while (std::getline(std::cin, line)) {
    input += line;
  }
  auto translator = JsonTranslator();
  auto pass_manager = translator.toIR(input);
  TDCEPass tdce;
  pass_manager.pass(tdce);
  std::cout << pass_manager << std::endl;
  translator.toStream(std::cout, pass_manager);
}
