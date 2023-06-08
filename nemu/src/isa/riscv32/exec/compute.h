#define EXI(idx, inst) case idx: \
		concat(rtl_,inst)(s, ddest, dsrc1, id_src2->imm); print_asm_template3(inst);break;
#define EXI_RELOP(idx, rtl, inst) case idx: \
		rtl_setrelopi(s, rtl, ddest, dsrc1, id_src2->imm); print_asm_template3(inst);break;
#define EXI_SH(idx, rtl, inst) case idx: \
		concat(rtl_, rtl)(s, ddest, dsrc1, id_src2->imm); print_asm_template3(inst); break;

#define EXR(idx, inst) case idx: \
		concat(rtl_, inst)(s, ddest, dsrc1, dsrc2); print_asm_template3(inst);break;
#define EXR_RELOP(idx, rtl, inst) case idx: \
		rtl_setrelop(s, rtl, ddest, dsrc1, dsrc2); print_asm_template3(inst);break;
#define EXR_SH(idx, rtl, inst) case idx: \
		concat(rtl_, rtl)(s, ddest, dsrc1, dsrc2); print_asm_template3(inst); break;

static inline def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
  print_asm_template2(lui);
}

