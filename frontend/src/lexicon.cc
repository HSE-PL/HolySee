#include "lexicon.h"

namespace hsec::frontend {

Keyword::operator const char*() const {
  switch (kind) {
    case kw_const:
      return "const";
    case kw_else:
      return "else";
    case kw_fun:
      return "fun";
    case kw_if:
      return "if";
    case kw_inside:
      return "inside";
    case kw_loop:
      return "loop";
    case kw_return:
      return "return";
    case kw_struct:
      return "struct";
    case kw_type:
      return "type";
    case kw_union:
      return "union";
    case kw_var:
      return "var";
  }
}

static const std::array<const Keyword, 11> values{
    kw_const,
    kw_else,
    kw_fun,
    kw_if,
    kw_inside,
    kw_loop,
    kw_return,
    kw_struct,
    kw_type,
    kw_union,
    kw_var,
};

}  // namespace hsec::frontend
