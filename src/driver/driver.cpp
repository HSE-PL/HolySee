#include "../../lib/argh.h"
#include "../ast_translator/translator.hpp"
#include "../ir/program.hpp"
#include "../ir/streamers/iostreamer.hpp"
#include "../lang/ast.hpp"
#include "../lexer/ilexer.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/iparser.hpp"
#include "../parser/parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::vector<Lexeme> lex(std::string &string, bool verbose) {
  auto lexer = LexerImpl();
  try {
    auto lexed = lexer.lex(string);
    if (verbose) {
      std::cout << "LEXING: " << std::endl;
      for (auto &&lexeme : lexed) {
        std::cout << lexeme.toString() << std::endl;
      }
    }
    return lexed;
  } catch (LexerException &exception) {
    std::cout << exception.error_msg << std::endl;
    throw;
  }
}

AST::TranslationUnit parse(std::vector<Lexeme> &lexems, bool verbose) {
  auto parser = AST::ParserImpl();
  try {
    auto parsed = parser.parse(lexems);
    if (verbose) {
      std::cout << "PARSING: " << std::endl;
      std::cout << parsed.toString() << std::endl;
    }
    return parsed;
  } catch (ParserException &exception) {
    std::cout << exception.error_msg << std::endl;
    throw;
  }
}

std::string getInput(char *path, bool verbose) {
  std::ifstream in(path);
  std::ostringstream input;
  input << in.rdbuf();
  std::string inputStr = input.str();
  if (verbose) {
    std::cout << inputStr << std::endl;
  }
  return inputStr;
}

IR::Program ir(AST::TranslationUnit &tu, bool verbose) {
  ASTTranslator translator;
  try {
    auto program = translator.translate(tu);
    if (verbose) {
      IR::IOStreamer io(std::cout);
      io.visit(program);
    }
    return program;
  } catch (std::string &str) {
    std::cout << "IR translation exception: " + str << std::endl;
    throw;
  }
}

std::shared_ptr<Machine::Mach> codegen(IR::Program &program, bool verbose) {
  auto filename = "lol.nasm";
  auto factory =
      std::make_shared<Machine::MFactoryNaive>(Machine::MFactoryNaive());
  auto mctx = Machine::MCtx(factory);
  auto emitted = program.emit(mctx);

  if (verbose) {
    std::ofstream os{filename};
    os << emitted->emit() << std::endl;
    os.close();
    /*std::cout << emitted->emit() << std::endl;*/
  }

  return emitted;
}

void run(int argc, char *argv[]) {
  auto cmdl = argh::parser(argc, argv);
  bool inputVerbose = false;
  bool lexerVerbose = false;
  bool parserVerbose = false;
  bool irVerbose = false;
  bool codegenVerbose = true;
  if (cmdl["--dump-input"]) {
    parserVerbose = true;
  }
  if (cmdl["--dump-parser"]) {
    parserVerbose = true;
  }
  if (cmdl["--dump-lexer"]) {
    lexerVerbose = true;
  }
  if (cmdl["--dump-ir"]) {
    irVerbose = true;
  }
  auto input = getInput(argv[1], inputVerbose);
  auto lexems = lex(input, lexerVerbose);
  auto parsed = parse(lexems, parserVerbose);
  auto program = ir(parsed, irVerbose);
  auto _ = codegen(program, codegenVerbose);
}
