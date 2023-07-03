#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV  // see /usr/include/elf.h to get the right type
#else
# error Unsupported ISA
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
	size_t len;
	Elf_Ehdr Ehdr;
	len = ramdisk_read(&Ehdr, 0, sizeof(Ehdr));
	assert(len == sizeof(Ehdr));
  assert(*(uint32_t *)Ehdr.e_ident == 0x464c457f);	// little endian
  assert(EXPECT_TYPE == Ehdr.e_machine);

	uint64_t phentoff = Ehdr.e_phoff;
	for (int i = 0; i < Ehdr.e_phnum; ++i) {
		Elf_Phdr Phdr;
	  len = ramdisk_read(&Phdr, phentoff, sizeof(Phdr));
	  assert(len == sizeof(Phdr));
		
		if (Phdr.p_type == PT_LOAD) {
			ramdisk_read((void*)Phdr.p_vaddr, Phdr.p_offset, Phdr.p_filesz);	
			memset((void*)(Phdr.p_vaddr + Phdr.p_offset), 0, Phdr.p_memsz - Phdr.p_filesz);
		}

		phentoff += Ehdr.e_phentsize;
	}
	
  return Ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

