//
// Created by goldolphin on 2022/6/12.
//

#pragma once

#include "definition.h"
#include <string>

namespace min {

enum class DirectiveCode : ByteT {
#define DEFINE_ITEM(id, code, str) id = code,
#include "min/def/directive.def"
#undef DEFINE_ITEM
};

std::string to_string(DirectiveCode di);
}
