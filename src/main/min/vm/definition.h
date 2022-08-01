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
class ReferenceType;
class Collectable;

typedef std::int32_t CountT;

typedef std::uint8_t ByteT;
typedef std::int64_t Int64T;

static_assert(sizeof(double) * CHAR_BIT == 64, "64-bit float is assumed.");
typedef double Float64T;

constexpr CountT OPERAND_WIDTH_4 = 4;

enum class Type : ByteT {
#define DEFINE_TYPE(id, code, name)  id = code,
  MIN_TYPES
#undef DEFINE_TYPE
};

enum class RetType : ByteT {
#define DEFINE_TYPE(id, code, name) id = static_cast<ByteT>(Type::id),
  MIN_RET_TYPES
#undef DEFINE_TYPE
};

enum class ValueType : ByteT {
#define DEFINE_TYPE(id, code, name) id = static_cast<ByteT>(Type::id),
  MIN_VALUE_TYPES
#undef DEFINE_TYPE
};

enum class PrimitiveType : ByteT {
#define DEFINE_TYPE(id, code, name) id = static_cast<ByteT>(Type::id),
  MIN_PRIMITIVE_TYPES
#undef DEFINE_TYPE
};

std::string type_to_string(Type type);
Result<RetType> to_ret_type(Type type);
Result<ValueType> to_value_type(Type type);
Result<PrimitiveType> to_primitive_type(Type type);

#pragma pack(push, 0)
struct min_byte_t {
  ByteT value;
};

struct min_int64_t {
  Int64T value;
};

struct min_float64_t {
  Float64T value;
};

struct min_proc_t {
  const Procedure* value;
};

struct min_type_t {
  const ReferenceType* value;
};

struct min_field_t {
  CountT value;
};

struct min_ref_t {
  Collectable* value;
};
#pragma pack(pop)

union Primitive {
  min_byte_t byte;
  min_int64_t int64;
  min_float64_t float64;
  min_proc_t proc;
  min_type_t type;
  min_field_t field;
};

union Value {
  Primitive primitive;
  min_ref_t reference;
};

}
