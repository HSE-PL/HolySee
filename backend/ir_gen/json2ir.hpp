#pragma once

#include "../ir/pass/pass_manager.hpp"
#include "json.hpp"

using namespace nlohmann;

PassManager json2ir(json &info);
