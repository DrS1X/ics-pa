#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csr[] = { "sstatus", "stvec", "sepc", "scause" };
const int csr_id[] = { 0x100, 0x105, 0x141, 0x142 };

void isa_reg_display() {
	for(int i = 0; i < 32; ++i)
		printf("%s: %#x; ", reg_name(i), (uint32_t)cpu.gpr[i]._32);
	printf("\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
	*success = true;
	for(int i = 0; i < 32; ++i){
		if(strcmp(s,reg_name(i)) == 0)
				return cpu.gpr[i]._32;
	}
	*success = false;
	return 0;
}

rtlreg_t *get_reg_ptr_by_name(const char *s) {
	for(int i = 0; i < 32; ++i)
		if(strcmp(s,reg_name(i)) == 0)
				return &cpu.gpr[i]._32;
	
	for (int i = 0; i < 4; ++i) 
		if (strcmp(s, csr[i]) == 0) 
			return &cpu.csr[i]._32;

	return NULL;
}
