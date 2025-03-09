#include "cli/cli.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  auto runner = Runner();
  runner.run(argc, argv);
}
