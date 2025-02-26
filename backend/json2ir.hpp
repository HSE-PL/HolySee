#pragma once

#include "backend.hpp"
#include "ir.hpp"
#include "json.hpp"

using namespace nlohmann;

vector<Function> json2ir(json &info);
