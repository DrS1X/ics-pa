#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__
#include <common.h>

// memory
#define riscv32_IMAGE_START 0x100000
#define riscv32_PMEM_BASE 0x80000000

// reg

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];
	
  struct {
    rtlreg_t _32;
  } csr[4];

  vaddr_t pc;	// pc is an isolate register that only store memory address
} riscv32_CPU_state;

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
			uint32_t imm11		 : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      int32_t  simm12    : 1;
    } b;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      int32_t simm31_12  :20;
    } u;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm19_12  : 8;
      uint32_t imm11	   : 1;
      uint32_t imm10_1   :10;
      int32_t simm20     : 1;	// sign
    } j;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;


#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define riscv32_has_mem_exception() (false)

#endif
