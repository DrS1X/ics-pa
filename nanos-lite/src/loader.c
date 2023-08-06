#include <proc.h>
#include <elf.h>
#include <fs.h>

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

	
static uintptr_t loader(PCB *pcb, const char *filename) {
	int fd = fs_open(filename, 0, 0);
	size_t len;
	Elf_Ehdr Ehdr;
	len = fs_read(fd, &Ehdr, sizeof(Ehdr));
	assert(len == sizeof(Ehdr));
  assert(*(uint32_t *)Ehdr.e_ident == 0x464c457f);	// little endian
  assert(EXPECT_TYPE == Ehdr.e_machine);

	uint64_t phentoff = Ehdr.e_phoff;
	for (int i = 0; i < Ehdr.e_phnum; ++i) {
		Elf_Phdr Phdr;
		fs_lseek(fd, phentoff, SEEK_SET);
	  len = fs_read(fd, &Phdr, sizeof(Phdr));
	  assert(len == sizeof(Phdr));
		
		if (Phdr.p_type == PT_LOAD) {
		  fs_lseek(fd, Phdr.p_offset, SEEK_SET);
			fs_read(fd, (void*)Phdr.p_vaddr, Phdr.p_filesz);	
			memset((void*)(Phdr.p_vaddr + Phdr.p_filesz), 0, Phdr.p_memsz - Phdr.p_filesz);
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

