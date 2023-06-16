#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
	int cmp1 = memcmp(cpu.gpr, ref_r, DIFFTEST_REG_SIZE / 8);
	int cmp2 = 0;//instr_fetch(cpu.pc, 4) - instr_fetch(pc, 4); 
  return cmp1 == 0 && cmp2 == 0 ? true : false;
}

void isa_difftest_attach() {
}
