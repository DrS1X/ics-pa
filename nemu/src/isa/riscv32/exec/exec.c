#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline def_EHelper(conditional_branches) {
	rtl_addi(s, s0, PC, id_dest->imm);
  switch (s->isa.instr.b.funct3) {
		EXB  (0b000, RELOP_EQ, beq)
		EXB  (0b001, RELOP_NE, bne)
    EXB  (0b100, RELOP_LT, blt)
    EXB  (0b101, RELOP_GE, bge)
    EXB  (0b110, RELOP_LTU, bltu)
    EXB  (0b111, RELOP_GEU, bgeu)
    default: exec_inv(s);
  }
}

static inline def_EHelper(compute_i) {
  switch (s->isa.instr.i.funct3) {
    EXI  (0b000, addi)
    EXI  (0b100, xori)
    EXI  (0b110, ori)
    EXI  (0b111, andi)
    EXI_RELOP  (0b010, RELOP_LT, slti)
		EXI_RELOP  (0b011, RELOP_LTU, sltiu)
		default:
			switch(id_src2->imm >> 5 << 3 | s->isa.instr.i.funct3){
				EXI_SH(0b0000000001, shli, slli)
				EXI_SH(0b0000000101, shri, srli)
				EXI_SH(0b0100000101, sari, srai)
				default: exec_inv(s);
			}
  }
}

static inline def_EHelper(compute_r) {
  switch (s->isa.instr.r.funct7 << 3 | s->isa.instr.r.funct3) {
    EXR  (0b0000000000, add)
    EXR  (0b0100000000, sub)
    EXR  (0b0000000100, xor)
    EXR  (0b0000000110, or)
    EXR  (0b0000000111, and)
    EXR_RELOP  (0b0000000010, RELOP_LT, slt)
    EXR_RELOP  (0b0000000011, RELOP_LTU, sltu)
    EXR_SH  (0b0000000001, shl, sll)
    EXR_SH  (0b0000000101, shr, srl)
    EXR_SH  (0b0100000101, sar, sra)
		
    EXR_M  (0b0000001000, mul_lo, mul)
    EXR_M  (0b0000001001, imul_hi, mulh)
    EXR_M  (0b0000001010, iumul_hi, mulhsu)
    EXR_M  (0b0000001011, mul_hi, mulhu)
    EXR_M  (0b0000001100, idiv_q, div)
		EXR_M  (0b0000001101, div_q, divu)
		EXR_M  (0b0000001110, idiv_r, rem)
		EXR_M  (0b0000001111, div_r, remu)
    default: exec_inv(s);
  }
}

static inline void set_width(DecodeExecState *s, int width) {
  if (width != 0) s->width = width;
}

static inline def_EHelper(load) {
  switch (s->isa.instr.i.funct3) {
    EXW  (0b000, ld, 1)
    EXW  (0b001, ld, 2)
    EXW  (0b010, ld, 4)
    EXW  (0b100, ld, 1)
    EXW  (0b101, ld, 2)
    default: exec_inv(s);
  }
}

static inline def_EHelper(store) {
  switch (s->isa.instr.s.funct3) {
    EXW  (0b000, st, 1)
    EXW  (0b001, st, 2)
    EXW  (0b010, st, 4)
    default: exec_inv(s);
  }
}

static inline def_EHelper(csr) {
  decode_op_r(s, id_src2, s->isa.instr.i.simm11_0, true);
  switch (s->isa.instr.i.funct3) {
		EX  (0b000, ecall_ebreak);
		EX  (0b001, csrrw);
		EX  (0b010, csrrs);
    default: exec_inv(s);
	}
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
  Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
  switch (s->isa.instr.i.opcode6_2) {
    IDEX (0b01101, U, lui)
    IDEX (0b00101, U, auipc)
    IDEX (0b11011, J, jal)
    IDEX (0b11001, I, jalr)
    IDEX (0b11100, I, csr);
    IDEX (0b11000, B, conditional_branches)
    IDEX (0b00000, I, load)
    IDEX (0b01000, S, store)
    IDEX (0b00100, I, compute_i)
		IDEX (0b01100, R, compute_r)
    EX   (0b11010, nemu_trap)
    default: exec_inv(s);
  }
}

static inline void reset_zero() {
  reg_l(0) = 0;
}

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  update_pc(&s);

  reset_zero();

  return s.seq_pc;
}
