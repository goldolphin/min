//
// Created by goldolphin on 2022/6/28.
//

#pragma once

#include "min/vm/assembly.h"
#include "min/common/result.h"
#include "min/vm/module_table.h"

namespace min::lib::io {

Result<void> LoadLibIo(ModuleTable* module_table);

}
