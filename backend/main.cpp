#include "cli/cli.hpp"

int main(int argc, char *argv[]) {
  auto runner = Runner();
  runner.run(argc, argv);
}
