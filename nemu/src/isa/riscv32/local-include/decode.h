#include <cpu/exec.h>
#include "rtl.h"
// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (DecodeExecState *s, Operand *op, word_t val, bool load_val)

static inline def_DopHelper(i) {	// The signed and unsigned immediate are both stored as uint32. 
  op->type = OP_TYPE_IMM;	// The signed immediate has been converted into two’s complement form,
	op->imm = val;					// so it can be calculated as an unsigned immediate.

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline def_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  op->preg = get_reg_ptr(val);

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg));
}

static inline def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, true);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, false);
}

static inline def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, true);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}

static inline def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, true);
  print_Dop(id_src2->str, OP_STR_SIZE, "0x%x", simm);
}

static inline def_DHelper(B) {
  sword_t simm = (s->isa.instr.b.simm12 << 12) | (s->isa.instr.b.imm11 << 11) |
								(s->isa.instr.b.imm10_5 << 5) | (s->isa.instr.b.imm4_1 << 1);
  decode_op_r(s, id_src1, s->isa.instr.b.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.b.rs2, true);
  decode_op_i(s, id_dest, simm, true);
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", simm);
}

static inline def_DHelper(U) {
  decode_op_r(s, id_dest, s->isa.instr.u.rd, false);
	sword_t simm = s->isa.instr.u.simm31_12 << 12; 
  decode_op_i(s, id_src1, simm, true);
  print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", simm);
}

static inline def_DHelper(J) {
	// offset[20|10:1|11|19:12]
  sword_t simm = (s->isa.instr.j.simm20 << 20) | (s->isa.instr.j.imm19_12 << 12) |
								(s->isa.instr.j.imm11 << 11) | (s->isa.instr.j.imm10_1 << 1);
  decode_op_i(s, id_src1, simm, true);
  decode_op_r(s, id_dest, s->isa.instr.j.rd, false);
  print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", simm);
}
