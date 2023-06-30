#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include <common.h>

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)

extern const int csr_id[];

static inline rtlreg_t* get_reg_ptr(int index) {
	if (index >= 0 && index < 32)
		return &cpu.gpr[index]._32;

	int i;
	for (i = 0; i < 4 && csr_id[i] != index; ++i);
	return &cpu.csr[i]._32;
}

static inline const char* reg_name(int index) {
  extern const char* regs[];
  if (index >= 0 && index < 32)
    return regs[index];
	
  extern const char* csr[];
	int i;
	for (i = 0; i < 4 && csr_id[i] != index; ++i);
	return csr[i];
}

#endif
