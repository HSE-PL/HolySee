#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;
using Parser = std::function<std::string(std::string_view)>;

template <typename Input>
class AbstractReader {
 public:
  virtual std::string read(Input inp) = 0;
  virtual ~AbstractReader() {}
};

class FileReader final : public AbstractReader<std::string> {
 public:
  std::string read(std::string fileName) override {
    auto size = fs::file_size(fileName);
    std::string ret(size, '\0');
    std::ifstream in(fileName);
    in.read(&ret[0], size);
    return ret;
  }
};

template <typename Ret>
class AbstractParserTester {
 public:
  virtual Ret testParser(
      Parser parser, std::string_view view, std::string ret
  ) = 0;
  virtual ~AbstractParserTester() {}
};

class SimpleParserTester final : public AbstractParserTester<bool> {
 public:
  virtual bool testParser(
      Parser parser, std::string_view view, std::string ret
  ) override {
    return parser(view) == ret;
  }
};

template <typename T>
concept Pushable = requires(std::vector<T> vec, T l) { vec.push_back(l); };

template <Pushable Ret>
class FrontendTestingHelper {
  std::shared_ptr<FileReader> freader;
  std::shared_ptr<AbstractParserTester<Ret>> tester;

 public:
  FrontendTestingHelper(
      std::shared_ptr<FileReader>& fr,
      std::shared_ptr<AbstractParserTester<Ret>> tester
  )
      : freader(fr), tester(tester) {}

  Ret testFile(std::string path, Parser parser) {
    auto str = freader->read(path);
    auto result = tester->testParser(parser, std::string_view(str), str);
    return result;
  }

  std::vector<Ret> testDir(std::string dirPath, Parser parser) {
    std::vector<Ret> ret{};

    for (const auto& file : fs::directory_iterator(dirPath)) {
      auto testResult = testFile(file.path(), parser);
      ret.push_back(testResult);
    }

    return ret;
  }
};
