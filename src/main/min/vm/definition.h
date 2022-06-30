//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "min/def/types.def"
#include "min/common/result.h"
#include <memory>
#include <cstdint>
#include <string>

namespace min {

class Procedure;
class Struct;
class StructValue;

typedef std::int32_t CountT;

typedef std::uint8_t ByteT;
typedef std::int64_t Int64T;

static_assert(sizeof(double) * CHAR_BIT == 64, "64-bit float is assumed.");
typedef double Float64T;

typedef const Procedure* ProcT;
typedef const Struct* TypeT;
typedef StructValue* RefT;

constexpr CountT OPERAND_WIDTH_4 = 4;

enum class Type : ByteT {
#define DEFINE_TYPE(id, code, str)  id = code,
  MIN_TYPES
#undef DEFINE_TYPE
};

enum class RetType : ByteT {
#define DEFINE_TYPE(id, code, str) id = static_cast<ByteT>(Type::id),
  MIN_RET_TYPES
#undef DEFINE_TYPE
};

enum class ValueType : ByteT {
#define DEFINE_TYPE(id, code, str) id = static_cast<ByteT>(Type::id),
  MIN_VALUE_TYPES
#undef DEFINE_TYPE
};

enum class PrimitiveType : ByteT {
#define DEFINE_TYPE(id, code, str) id = static_cast<ByteT>(Type::id),
  MIN_PRIMITIVE_TYPES
#undef DEFINE_TYPE
};

std::string type_to_string(Type type);
Result<RetType> to_ret_type(Type type);
Result<ValueType> to_value_type(Type type);
Result<PrimitiveType> to_primitive_type(Type type);

union Primitive {
  ByteT byte_value;
  Int64T int64_value;
  Float64T float64_value;
  ProcT proc_value;
  TypeT type_value;
  CountT field_value;
};

union Value {
  Primitive primitive;
  RefT reference;
};

}
