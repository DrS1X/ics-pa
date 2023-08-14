#include <common.h>
#include <stdarg.h>

#define LOG_MAX  (2 * 1024 * 1024)

Func func_tab[FUNC_TAB_SIZE];

FILE *log_fp = NULL;
FILE *ftrace_log_fp = NULL;

void init_log(const char *log_file, const char * ftrace_log_file) {
  if (log_file == NULL) return;
  log_fp = fopen(log_file, "w");
  Assert(log_fp, "Can not open '%s'", log_file);

  if (ftrace_log_file == NULL) return;
  ftrace_log_fp = fopen(ftrace_log_file, "w");
  Assert(ftrace_log_fp, "Can not open '%s'", ftrace_log_file);

}

char log_bytebuf[80] = {};
char log_asmbuf[80] = {};
char log_ftrace_buf[128] = {};
static char tempbuf[256] = {};

void strcatf(char *buf, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  strcat(buf, tempbuf);
}

void asm_print(vaddr_t this_pc, int instr_len, bool print_flag) {
	static uint64_t log_size = 0;
	if (++log_size > LOG_MAX) {
		rewind(log_fp);
		log_size = 0;
	}

  snprintf(tempbuf, sizeof(tempbuf), FMT_WORD ":   %s%*.s%s", this_pc, log_bytebuf,
      50 - (12 + 3 * instr_len), "", log_asmbuf);
  log_write(log_fp, "%s\n", tempbuf);
  if (print_flag) {
    puts(tempbuf);
  }

  log_bytebuf[0] = '\0';
  log_asmbuf[0] = '\0';
}

void ftrace_print(vaddr_t this_pc) {
	static uint64_t log_size = 0;
	if (++log_size > LOG_MAX) {
		rewind(ftrace_log_fp);
		log_size = 0;
	}

	static size_t stack_depth = 0;
	if (log_ftrace_buf[0] == '\0') return;

	if (log_ftrace_buf[0] == 'r') stack_depth--; // ret
  snprintf(tempbuf, sizeof(tempbuf), FMT_WORD ": %*.s%s", this_pc, (int)stack_depth, "", log_ftrace_buf);
	if (log_ftrace_buf[0] == 'c') stack_depth++; // call

	log_ftrace_buf[0] = '\0';
  log_write(ftrace_log_fp, "%s\n", tempbuf);
}

void end_of_log_file() {
	log_write(log_fp, "^EOF\n");
	log_write(ftrace_log_fp, "^EOF\n");
}
