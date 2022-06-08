//
// Created by caofuxiang on 2021/5/20.
//

#pragma once

#include "definition.h"
#include <string>

namespace min {

enum class OpCode : ByteT {
  NOP = 0,             // No operation

  // 求值栈相关
  POPP = 1,            //                   value ->              弹出并丢弃栈顶primitive值
  POPR = 2,            //                   value ->              弹出并丢弃栈顶reference值

  // 常量相关
  LOADC = 3,           // const_index       -> value              将primitive常量值压栈
  LOADN = 4,           //                   -> null               将reference常量null压栈

  // 局部变量相关
  LOADP = 5,           // var_index         -> value              将primitive变量压栈
  LOADR = 6,           // var_index         -> value              将reference变量压栈
  STOREP = 7,          // var_index         value ->              弹出栈顶primitive值到变量
  STORER = 8,          // var_index         value ->              弹出栈顶reference值到变量

  // 过程调用
  CALL = 9,            //        procedure, [arg] -> value        调用过程
  RET = 10,            //                [value] -> []            退出过程

  // 控制结构
  IF = 13,             // offset            value ->              判断跳转，注意 offset 是过程内的绝对偏移
  GOTO = 14,           // offset            ->                    跳转，注意 offset 是过程内的绝对偏移

  // 对象操作
  NEW = 15,            //                   type -> obj_ref       分配指定类型的对象，并将其 reference 值压栈
  SINGLETON = 16,      //                   type -> obj_ref       获取指定类型的单例对象，并将其 reference 值压栈
  TYPEOF = 17,         //                   obj_ref -> type       获取指定对象的 type
  GETP = 18,           //             obj_ref, field -> value     读对象指定 primitive 字段
  GETR = 19,           //             obj_ref, field -> value     读对象指定 reference 字段
  SETP = 20,           //             obj_ref, field, value ->    写对象指定 primitive 字段
  SETR = 21,           //             obj_ref, field, value ->    写对象指定 reference 字段

  // 引用运算
  EQ_REF,

  // 整数运算
  NEG_INT64,
  ADD_INT64,
  SUB_INT64,
  MUL_INT64,
  DIV_INT64,
  MOD_INT64,

  NOT_INT64,
  AND_INT64,
  OR_INT64,
  XOR_INT64,
  SHL_INT64,
  SHR_INT64,

  EQ_INT64,
  NE_INT64,
  GT_INT64,
  GE_INT64,
  LT_INT64,
  LE_INT64,

  // 浮点数运算
  NEG_DOUBLE,
  ADD_DOUBLE,
  SUB_DOUBLE,
  NUL_DOUBLE,
  DIV_DOUBLE,
  MOD_DOUBLE,

  EQ_DOUBLE,
  NE_DOUBLE,
  GT_DOUBLE,
  GE_DOUBLE,
  LT_DOUBLE,
  LE_DOUBLE,

  // 数值类型转换
  CONV_INT64_DOUBLE,
  CONV_DOUBLE_INT64,

};

}