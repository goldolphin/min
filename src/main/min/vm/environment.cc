//
// Created by goldolphin on 2022/4/11.
//
#include "environment.h"

namespace min {

Environment::Environment(CallStack* call_stack, Heap* heap, ModuleTable* module_table)
    : call_stack_(call_stack), heap_(heap), module_table_(module_table) {}

}