#pragma once

#include "backend.hpp"
#include "ir.hpp"
#include "json.hpp"

using namespace nlohmann;

PassManager json2ir(json &info);
