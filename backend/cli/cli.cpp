#include "cli.hpp"
/*#include "../opt/tdce.hpp"*/
#include "argh.h"
/**/
/*#include "../ir_gen/json/json_translator.hpp"*/
#include "../ir/try/jsontranslator.hpp"
#include "../ir/try/streamers/iostreamer.hpp"
#include <iostream>

void Runner::run(int argc, char *argv[]) {
  auto cmdl = argh::parser(argc, argv);
  auto opts_enabled = cmdl[{"O1", "opt"}];
  std::string input;
  std::string line;
  while (std::getline(std::cin, line)) {
    input += line;
  }
  auto ir = toIR(input);
  auto io = IOStreamer(std::cout);
  ir.accept(io);
  /*auto translator = JsonTranslator();*/
  /*auto pass_manager = translator.toIR(input);*/
  /*TDCEPass tdce;*/
  /*std::cout << pass_manager << std::endl;*/
  /*pass_manager.pass(tdce);*/
  /*std::cout << pass_manager << std::endl;*/
  /*translator.toStream(std::cout, pass_manager);*/
}
