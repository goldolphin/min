//
// Created by goldolphin on 2022/6/10.
//

#include "op.h"

namespace min {

std::string to_string(OpCode op) {
  switch (op) {
#define DEFINE_ITEM(id, code, str) case OpCode::id: return str;
#include "min/def/op.def"
#undef DEFINE_ITEM
  }
}

}