#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include "memory/paddr.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
	int n = 1;
	char * n_str = strtok(NULL, " ");
	if(n_str)
		n = strtol(n_str, NULL, 10);
	cpu_exec(n);
	return 0;
}

static int cmd_w(char *args){
		new_wp(args);
		return 0;
}

static int cmd_d(char *args){
	char * n_str = strtok(NULL, " ");
	if(n_str){
		int NO = strtol(n_str, NULL, 10);
		free_wp(NO);
	}else
			printf("Watchpoint NO is missing.\n");
	return 0;
}
static int cmd_help(char *args);
static int cmd_p(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute single step", cmd_si },
  { "info", "Print information of program status", cmd_info },
  { "p", "Expression", cmd_p },
	{ "x", "Scan memory", cmd_x },
	{ "w", "Create watchpoint with expression", cmd_w },
	{ "d", "Delete watchpoint with NO", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	char cmd = arg ? *arg : 0;
	switch(cmd) {
		case 'r':
			isa_reg_display();
			break;
		case 'w':
			display_wp();
			break;
		default:
			printf("Unknown command '%c'\n", cmd);

	}
	return 0;
}

static int cmd_p(char *args){
	bool success;
	word_t res = expr(args, &success);
	if(success)
		printf("%d\n", res);
	else
		printf("Fail to evaluate the expression.\n");
	return 0;
}

static int cmd_x(char *args){
	errno = 0;
	char * endptr, *n_str = strtok(NULL, " ");
	long N = strtol(n_str, &endptr, 10);
	if ((errno == ERANGE && (N == LONG_MAX || N == LONG_MIN)) || (errno != 0 && N == 0)) {
		perror("strtol");  
		return 0;
	}
	if (endptr == n_str) {
		fprintf(stderr, "No digits were found\n");
		return 0;
	}

	bool success;
	paddr_t paddr = expr(strtok(NULL, " "), &success);
	for(int i = 0; i < N; ++i){
			paddr += i * 4; 
			word_t word = paddr_read(paddr, 4);
			printf("%x ", word);
	}
	printf("\n");
	return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
