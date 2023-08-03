#define EXB(idx, rtl, inst) case idx: rtl_jrelop(s, rtl, dsrc1, dsrc2, *s0); print_asm_template3(inst); break;

// The s.seq_pc will be pushed foward after fetch an instruct at
//  ~/ics2020/nemu/include/cpu/exec.h	: 33
#define PC (rtlreg_t *)&cpu.pc
#define PC_4 (rtlreg_t *)&s->seq_pc

static inline def_EHelper(auipc) {
	rtl_addi(s, ddest, PC, id_src1->imm);
	print_asm_template2(auipc);
}

// x[rd] = pc+4; pc += sext(offset)
static inline def_EHelper(jal) {
	rtl_add(s, ddest, PC_4, rz);

	rtl_addi(s, s0, PC, id_src1->simm);
	rtl_jr(s, s0);	// s->jmp_pc = s0

	extern char log_ftrace_buf[];
	if (strcmp(id_dest->str, "ra") == 0)
		sprintf(log_ftrace_buf, "call [%s@%x]", get_func_name(*s0), *s0);

  print_asm_template2(jal);
}

// t =pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
static inline def_EHelper(jalr) {
	rtl_addi(s, s0, dsrc1, id_src2->imm);
	rtl_andi(s, s0, s0, -2);	// complement of -2 is equal to ~1
	rtl_jr(s, s0);

	rtl_add(s, ddest, PC_4, rz);

	extern char log_ftrace_buf[];
	if (strcmp(id_dest->str, "ra") == 0)
		sprintf(log_ftrace_buf, "call [%s@%x]", get_func_name(*s0), *s0);
	else if (strcmp(id_src1->str, "ra") == 0)
		sprintf(log_ftrace_buf, "ret [%s]", get_func_name(*dsrc1));

  print_asm_template3(jalr);
}
