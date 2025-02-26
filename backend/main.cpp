#include "json.hpp"
#include "json2ir.hpp"
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
  json inpJson = json::parse(input);
  auto fns = json2ir(inpJson);
}
