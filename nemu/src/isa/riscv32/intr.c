#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc) {
  /* Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	rtlreg_t *sepc = get_reg_ptr_by_name("sepc");	
	rtlreg_t *scause = get_reg_ptr_by_name("scause");	
	rtlreg_t *stvec = get_reg_ptr_by_name("stvec");	
	*sepc = epc;
	*scause = NO;
	s->is_jmp = true;
	s->jmp_pc = *stvec;
}

void query_intr(DecodeExecState *s) {
}
