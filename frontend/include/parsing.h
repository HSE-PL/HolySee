#ifndef HSEC_FRONTEND_PARSING_H
#define HSEC_FRONTEND_PARSING_H

#include "lexicon.h"

namespace hsec::frontend::parsing {

template <typename T>
concept ParseStream = requires(T& buf, Token token) { token = buf.next(); };

template <typename T>
concept ParseBuffer = ParseStream<T>
                      && requires(T& buf, Token token) { token = buf.peek(); };

template <ParseStream T>
class SingleTokenBuffer {
 public:
  typedef std::optional<Token> Buffer;

 private:
  T& stream;
  Buffer& buf;

 public:
  SingleTokenBuffer(T& stream, Buffer&& buf = Buffer{})
      : stream(stream), buf(buf) {}
};

// template <ParseStream T>
// class ParseBuffer {
//   T& stream;
//   std::deque<Token> buf;

//   explicit ParseBuffer(T& stream) : stream(stream) {}

//   Token next() {}

//   const Token& peek(size_t n = 0) {
//     while (buf.size() <= n)
//       buf.push_back(next());
//   }
// };

}  // namespace hsec::frontend::parsing

#endif
