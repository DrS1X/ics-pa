void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

static inline def_EHelper(privileged_inst) {
	switch (id_src2->imm) {
		case 0b000000000000:
			raise_intr(s, 8, cpu.pc); 
			print_asm_template1(ecall);
			break;
		case 0b000100000010: {
			rtlreg_t *sepc = get_reg_ptr_by_name("sepc");
			rtl_jr(s, sepc);	
			print_asm_template1(sret);
			break;
	  }
		default:
			assert(0);
	}
}

static inline def_EHelper(csrrw) {
  rtl_mv(s, ddest, dsrc2);
  rtl_mv(s, dsrc2, dsrc1);
  print_asm_template3(csrrw);
}

static inline def_EHelper(csrrs) {
  rtl_mv(s, ddest, dsrc2);
  rtl_or(s, dsrc2, dsrc2, dsrc1);
  print_asm_template3(csrrs);
}
