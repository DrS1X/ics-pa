#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

const char binary_opt[][3] = {
	"+", "-", "*", "/",
	"==", "!=",
	"&&"
};
const int nr_binary_opt = sizeof(binary_opt) / sizeof(binary_opt[0]);

static void gen_rand_expr(int *depth) {
	uint32_t num;
	int choose = rand() % 3;
	if(++(*depth) > 10)
			choose = 0;
	else if( *depth < 3)
			choose = 2;

	switch(choose){
	  case 0:
				num = rand();
				sprintf(buf + strlen(buf), "%u", num);
				break;
		case 1:
			buf[strlen(buf) + 1] = '\0';
			buf[strlen(buf)] = '(';
			gen_rand_expr(depth);
			buf[strlen(buf) + 1] = '\0';
			buf[strlen(buf)] = ')';
			break;
	  default:
			gen_rand_expr(depth);
			strcat(buf + strlen(buf), binary_opt[rand() % nr_binary_opt]);	// rand operation
			gen_rand_expr(depth);
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		code_buf[0] = '\0';
		buf[0] = '\0';

		int depth = 0;
    gen_rand_expr(&depth);

    sprintf(code_buf, code_format, buf);
	
		FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

		fp = popen("gcc /tmp/.code.c -o /tmp/.expr 2>&1", "r");
		assert(fp != NULL);
		char msg[1024] = {0};
		size_t n_rec = fread(msg, 1, sizeof(msg), fp);
		pclose(fp);
		if(n_rec != 0){
				//printf("Throw the expression. Warning: %s\n", msg);
				continue;
		}

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);
    int result;
    assert(fscanf(fp, "%d", &result) == 1);
    pclose(fp);
	
    printf("p %u - (%s)\n", result, buf);
  } 
  return 0;
}
