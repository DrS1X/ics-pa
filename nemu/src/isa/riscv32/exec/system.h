void raise_intr(DecodeExecState *s, word_t NO, vaddr_t epc);

static inline def_EHelper(ecall_ebreak) {
	if (id_src2->imm) {
    print_asm_template1(ebreak);
	}
	else {
		raise_intr(s, 8, cpu.pc);
    print_asm_template1(ecall);
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
