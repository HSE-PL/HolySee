#include "vtype.hpp"
#include "../../backend.hpp"

std::unordered_map<std::string, VType> s2t = {
    {"int", VType::Int},
    {"bool", VType::Bool},
    {"var", VType::Ref},
    {"unit", VType::Unit},
    //
};

std::unordered_map<VType, std::string> t2s = {
    {VType::Int, "int"},
    {VType::Unit, "unit"},
    {VType::Ref, "var"},
    {VType::Bool, "bool"},
    //
};

VType string2type(std::string type) {
  if (!s2t.contains(type)) {
    throw NotImplemented(" such type doesn't exist " + type);
  }
  return s2t.at(type);
}
