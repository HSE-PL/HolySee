#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
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
  const std::string inputPrefix = "input";
  const std::string expectedPrefix = "expected-";

  std::string expectedPath(size_t testNum) {}

  std::optional<std::string> testName(std::string input) {
    if (input.length() <= inputPrefix.length())
      return std::nullopt;

    auto res = std::mismatch(
        inputPrefix.begin(), inputPrefix.end(), input.begin()
    );

    if (res.first == inputPrefix.end() && *res.second == '-') {
      auto testStr = ++res.second;
      std::string ret{};

      if (testStr == input.end()) {
        return std::nullopt;
      }

      while (testStr != input.end()) {
        ret.push_back(*testStr++);
      }
      return ret;
    }
    return std::nullopt;
  }

 public:
  FrontendTestingHelper(
      std::shared_ptr<FileReader>& fr,
      std::shared_ptr<AbstractParserTester<Ret>> tester
  )
      : freader(fr), tester(tester) {}

  Ret testFile(std::string expectedPath, std::string filePath, Parser parser) {
    auto str = freader->read(filePath);
    auto expected = freader->read(expectedPath);
    auto result = tester->testParser(parser, std::string_view(str), expected);
    return result;
  }

  std::vector<Ret> testDir(std::string dirPath, Parser parser) {
    std::vector<Ret> ret{};
    auto dir = fs::path(dirPath);

    for (const auto& file : fs::directory_iterator(dirPath)) {
      auto fname = fs::path(file.path()).filename();

      if (auto postfix = testName(fname)) {
        auto expectedFile = dir / (expectedPrefix + *postfix);
        auto expected = freader->read(expectedFile);

        auto input = freader->read(file.path());

        auto testResult = tester->testParser(
            parser, std::string_view(input), expected
        );

        ret.push_back(testResult);
      }
    }

    return ret;
  }
};
