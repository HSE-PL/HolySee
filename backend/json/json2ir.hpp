#pragma once

#include "../ir/ir.hpp"
#include "json.hpp"

using namespace nlohmann;

PassManager json2ir(json &info);
