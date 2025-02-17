#pragma once

#include "common.h"

void wait(u64 cycles);
u64 get_cur_sp();
u64 get_base_sp();
u64 get_stack_size();
void call_dyn_func();
