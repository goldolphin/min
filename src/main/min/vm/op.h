//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "definition.h"
#include <string>

namespace min {

enum class OpCode : ByteT {
#define DEFINE_ITEM(id, code, str) id = code,
#include "min/def/op.def"
#undef DEFINE_ITEM
};

std::string to_string(OpCode op);
}
