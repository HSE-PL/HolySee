#include "cli.hpp"
#include "../ir/json/jsontranslator.hpp"
#include "../ir/streamers/iostreamer.hpp"
#include "argh.h"
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
}
