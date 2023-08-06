#include <isa.h>
#include <memory/paddr.h>
#include <monitor/monitor.h>
#include <getopt.h>
#include <stdlib.h>
#include <elf.h>
#include <assert.h>

#if defined(__ISA_native__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_riscv32__)
# define EXPECT_TYPE EM_RISCV  // see /usr/include/elf.h to get the right type
#else
# error Unsupported ISA
#endif

#ifdef ISA64 
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Shdr Elf64_Shdr 
# define Elf_Sym Elf64_Sym 
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Shdr Elf32_Shdr
# define Elf_Sym Elf32_Sym 
#endif

void init_log(const char *log_file, const char *ftrace_log_file);
void init_mem();
void init_regex();
void init_wp_pool();
void init_difftest(char *ref_so_file, long img_size, int port);

static char log_file[256] = {};
static char ftrace_log_file[256] = {};
static char *diff_so_file = NULL;
static char *img_file = NULL;
static char *img_elf[5];
static int img_elf_i = 0;
static int batch_mode = false;
static int difftest_port = 1234;

int is_batch_mode() { return batch_mode; }

static inline void welcome() {
#ifdef DEBUG
  Log("Debug: \33[1;32m%s\33[0m", "ON");
  Log("If debug mode is on, A log file will be generated to record every instruction NEMU executes. "
      "This may lead to a large log file. "
      "If it is not necessary, you can turn it off in include/common.h.");
#else
  Log("Debug: \33[1;32m%s\33[0m", "OFF");
#endif

  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to \33[1;41m\33[1;33m%s\33[0m-NEMU!\n", str(__ISA__));
  printf("For help, type \"help\"\n");
}

static inline long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  Log("The image is %s", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(IMAGE_START), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static inline void init_function_table() {
	extern Func func_tab[]; 
	int func_i = 0;
	for (img_elf_i--; img_elf_i >= 0; img_elf_i--) {
		FILE *fp = fopen(img_elf[img_elf_i], "rb");
		assert(fp);
		long elf_off = 0;
		while (1) {
			size_t rn;
			Elf_Ehdr Ehdr;
			fseek(fp, elf_off, SEEK_SET);
			rn = fread(&Ehdr, sizeof(Ehdr), 1, fp);

			if (rn != 1 || *(uint32_t *)Ehdr.e_ident != 0x464c457f)	// little endian
				break;
			assert(EXPECT_TYPE == Ehdr.e_machine);
			elf_off += Ehdr.e_shoff + Ehdr.e_shnum * Ehdr.e_shentsize;
			
			uintptr_t symtab_off = 0, symtab_entsize = 0, symtab_end = 0;
			uintptr_t strtab_off = 0; 
			uintptr_t shentoff = Ehdr.e_shoff;
			uintptr_t sh_end = Ehdr.e_shoff + Ehdr.e_shentsize * Ehdr.e_shnum;
			while (shentoff < sh_end) { 
				Elf_Shdr Shdr;
				fseek(fp, shentoff, SEEK_SET);
				rn = fread(&Shdr, sizeof(Shdr), 1, fp);
				assert(rn == 1); 

				if (Shdr.sh_type == SHT_SYMTAB) {
					symtab_off = Shdr.sh_offset;
					symtab_entsize = Shdr.sh_entsize;
					symtab_end = Shdr.sh_offset + Shdr.sh_size;

					fseek(fp, Ehdr.e_shoff + Shdr.sh_link * Ehdr.e_shentsize, SEEK_SET);
					rn = fread(&Shdr, sizeof(Shdr), 1, fp);
					assert(rn == 1); 
					strtab_off = Shdr.sh_offset;
				}
				shentoff += Ehdr.e_shentsize;
			}

			Elf_Sym sym;
			while (symtab_off < symtab_end && func_i < FUNC_TAB_SIZE - 1) {
				fseek(fp, symtab_off, SEEK_SET);
				rn = fread(&sym, sizeof(sym), 1, fp);	
				assert(rn == 1); 
				if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC) {
					assert(func_i < FUNC_TAB_SIZE);
					func_tab[func_i].start = sym.st_value;
					func_tab[func_i].end = sym.st_value + sym.st_size;

					fseek(fp, strtab_off + sym.st_name, SEEK_SET);
					rn = fread(func_tab[func_i].name, 16, 1, fp);	
					assert(rn == 1); 
					++func_i;
				}
				symtab_off += symtab_entsize;
			}
		}
	}
	func_tab[func_i].end = 0;
	strcpy(func_tab[func_i].name, "???");

	extern FILE *ftrace_log_fp;
	fprintf(ftrace_log_fp, "Function Table\n");
	for (int i = 0; i < FUNC_TAB_SIZE && func_tab[i].end != 0; ++i) {
		fprintf(ftrace_log_fp, "[%lx, %lx] %s\n",  
				func_tab[i].start, func_tab[i].end, func_tab[i].name); 
	}
	fprintf(ftrace_log_fp, "Function Table\n\n");

	return;
}

static inline void parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
		{"elf"			, required_argument, NULL, 'e'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
      case 'b': batch_mode = true; break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': {
					strcpy(log_file, optarg);
					strcpy(ftrace_log_file, optarg);
					strcat(log_file, "nemu-log.txt");
					strcat(ftrace_log_file, "ftrace-nemu-log.txt");
					break;
			}
      case 'd': diff_so_file = optarg; break;
			case 'e': img_elf[img_elf_i++] = optarg; break;
      case 1:
        if (img_file != NULL) Log("too much argument '%s', ignored", optarg);
				else 
					img_file = optarg;
        break;
      default:
        printf("Usage: %s [OPTION...] IMAGE\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=DIR            output log to DIR\n");
        printf("\t-e,--elf=FILE           parse FILE to get symbol table\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Open the log file. */
  init_log(log_file, ftrace_log_file);

  /* Fill the memory with garbage content. */
  init_mem();

  /* Perform ISA dependent initialization. */
  init_isa();

	/* Load the ELF file of image for function trace. */
  init_function_table();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Display welcome message. */
  welcome();
}
