#include "lexer/ilexer.hpp"
#include "lexer/lexer.hpp"
#include "parser/iparser.hpp"
#include "parser/parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "this sheise takes exactly 1 argument which is filename";
    exit(228);
  }
  std::ifstream in(argv[1]);
  std::ostringstream input;
  input << in.rdbuf();
  std::string inputStr = input.str();
  std::cout << inputStr << std::endl;
  auto lexer = LexerImpl();
  auto parser = ParserImpl();
  try {
    auto lexed = lexer.lex(inputStr);
    /*std::cout << "LEXING: " << std::endl;*/
    /*for (auto &&lexeme : lexed) {*/
    /*  std::cout << lexeme.toString() << std::endl;*/
    /*}*/
    auto parsed = parser.parse(lexed);
    std::cout << "PARSING: " << std::endl;
    std::cout << parsed.toString() << std::endl;
  } catch (ParserException &exception) {
    std::cout << exception.error_msg << std::endl;
  } catch (LexerException &exception) {
    std::cout << exception.error_msg << std::endl;
  }
  return 0;
}
