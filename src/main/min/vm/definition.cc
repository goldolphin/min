//
// Created by goldolphin on 2022/6/10.
//

#include "definition.h"

namespace min {

std::string type_to_string(Type type) {
  switch (type) {
#define DEFINE_TYPE(id, code, name) case Type::id: return #name;
    MIN_TYPES
#undef DEFINE_TYPE
    default:
      return "unknown_type";
  }
}

Result<RetType> to_ret_type(Type type) {
  switch (type) {
#define DEFINE_TYPE(id, code, name) case Type::id: return RetType::id;
    MIN_RET_TYPES
#undef DEFINE_TYPE
    default:
      return make_error("Not a ret type: " + type_to_string(type));
  }
}

Result<ValueType> to_value_type(Type type) {
  switch (type) {
#define DEFINE_TYPE(id, code, name) case Type::id: return ValueType::id;
    MIN_VALUE_TYPES
#undef DEFINE_TYPE
    default:
      return make_error("Not a value type: " + type_to_string(type));
  }
}

Result<PrimitiveType> to_primitive_type(Type type) {
  switch (type) {
#define DEFINE_TYPE(id, code, name) case Type::id: return PrimitiveType::id;
    MIN_PRIMITIVE_TYPES
#undef DEFINE_TYPE
    default:
      return make_error("Not a primitive type: " + type_to_string(type));
  }
}
}