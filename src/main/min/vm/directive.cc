//
// Created by goldolphin on 2022/6/10.
//

#include "directive.h"

namespace min {

std::string to_string(DirectiveCode di) {
  switch (di) {
#define DEFINE_ITEM(id, code, str) case DirectiveCode::id: return str;
#include "min/def/directive.def"
#undef DEFINE_ITEM
  }
}

}