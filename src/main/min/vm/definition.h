//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include <memory>
#include <cstdint>

namespace min {

class Procedure;
class Struct;
class StructValue;

typedef std::int32_t CountT;

typedef std::uint8_t ByteT;
typedef std::int64_t Int64T;
typedef double DoubleT;
typedef const Procedure* ProcT;
typedef const Struct* TypeT;
typedef StructValue* RefT;

constexpr CountT OPERAND_WIDTH_4 = 4;

namespace {
struct Types {
  enum {
    // Void type
    VOID = 0,

    // Primitive type
    BYTE = 1,
    INT64 = 2,
    DOUBLE = 3,
    PROCEDURE = 4,
    TYPE = 5,
    FIELD = 6,

    // RefT type
    REFERENCE = 7,
  };
};
}

enum class ValueType {
  BYTE = Types::BYTE,
  INT64 = Types::INT64,
  DOUBLE = Types::DOUBLE,
  PROCEDURE = Types::PROCEDURE,
  TYPE = Types::TYPE,
  FIELD = Types::FIELD,
  REFERENCE = Types::REFERENCE,
};

enum class PrimitiveType {
  BYTE = Types::BYTE,
  INT64 = Types::INT64,
  DOUBLE = Types::DOUBLE,
  PROCEDURE = Types::PROCEDURE,
  TYPE = Types::TYPE,
  FIELD = Types::FIELD,
};

union Primitive {
  ByteT byte_value;
  Int64T int64_value;
  DoubleT double_value;
  ProcT procedure_value;
  TypeT type_value;
  CountT field_value;
};

union Value {
  Primitive primitive;
  RefT reference;
};

}
