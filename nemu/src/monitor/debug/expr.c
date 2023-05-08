#include <isa.h>
#include <memory/paddr.h>
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_REG, TK_EQ, TK_NEQ, TK_AND, TK_NUM, TK_DEREF,
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // subtract
  {"\\*", '*'},         // mutiple or dereference
  {"/", '/'},			// divide
  {"\\(", '('},         // left parentheses
  {"\\)", ')'},         // right parentheses
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},		// not equal
  {"&&", TK_AND},		// and
  {"([0-9]+)|(0x[0-9a-f]+)", TK_NUM}, // decimal or hexadecimal number
  {"\\$(\\$0|[0-9a-zA-Z]+)", TK_REG}, // register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static int token_precedence(const Token token){
	switch(token.type){
		case TK_DEREF:
			return 0;
		case '*': case '/':
			return 1;
		case '+': case '-':
			return 2;
		case TK_EQ: case TK_NEQ:
			return 3;
		case TK_AND:
			return 4;
		case '(': case ')':
			return 5;
		default:
			return -1;
	}
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		Token *tk = &tokens[nr_token];
        switch (rules[i].token_type) {
			case TK_NUM:
				tk->type = TK_NUM;
				strncpy(tk->str, substr_start, substr_len);
				tk->str[substr_len] = 0;
				if(*substr_start == '0' && *(substr_start + 1) == 'x'){
					unsigned long hexnum = strtol(tk->str, NULL, 16);
					sprintf(tk->str, "%lu", hexnum);
				}
				break;
			case TK_REG:
				tk->type = TK_REG;
				strncpy(tk->str, substr_start + 1, substr_len - 1);
				tk->str[substr_len - 1] = 0;
				break;
			case '*':
				if(nr_token > 0 && (tokens[nr_token - 1].type == TK_NUM || 
					tokens[nr_token - 1].type == TK_REG || tokens[nr_token - 1].type == ')'))
						tk->type = '*';
				else
					tk->type = TK_DEREF;
				break;
			case TK_NOTYPE:
				--nr_token;
				break;
			default: 
				tk->type = rules[i].token_type;
       }
		++nr_token;

		break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// check if the whole expression is in a pair of parentheses
bool check_parentheses(int l, int r, bool *valid){
		*valid = true;
		int nr_left_prt = 0;
		for(int i = l; i < r; ++i){
				if(tokens[i].type == '('){
					++nr_left_prt;
				}else if(tokens[i].type == ')'){
					--nr_left_prt;
					if(nr_left_prt < 0){
						printf("No matched left parentheses for right parentheses. the right parentheses is at %d.\n", i);
						*valid = false;
						return false;
					}else if(nr_left_prt == 0){
						return false;
					}
				}
		}
		if(nr_left_prt == 1)		
				return true;
		else if(nr_left_prt > 1){
			printf("No matched right parentheses for left parentheses. the number of unmatched left parentheses is %d.\n", nr_left_prt);
			*valid = false;
			return false;
		}else
				return false;
}

int search_pivot(int l, int r){
		int pivot = -1, nr_left_prt = 0;
		for(int i = l; i < r; ++i){
				if(tokens[i].type == TK_NUM)
						continue;
				else if(tokens[i].type == '('){
					++nr_left_prt;
				}else if(tokens[i].type == ')'){
					--nr_left_prt;
				}else{
						if(nr_left_prt > 0)
								continue;
						if(pivot == -1 || token_precedence(tokens[i]) >= token_precedence(tokens[pivot]))
								pivot = i;
				}
		}
		return pivot;
}

int eval(int l, int r, bool *success){
	*success = false;

	if(l > r){
		printf("Invalid index. left index: %d, right index: %d.\n", l, r);
		return 0;
	}else if(l == r){
		if(tokens[l].type == TK_NUM){
			int res = atoi(tokens[l].str);
			*success = true;
			return res;
		}else if(tokens[l].type == TK_REG){
			return isa_reg_str2val(tokens[l].str, success);
		}else{
			printf("Unknown single token. token: %s.\n", tokens[l].str);
			return 0;
		}
	}else if(tokens[l].type == '(' && tokens[r].type == ')'){
		bool valid;
		if(check_parentheses(l, r, &valid)){
			return eval(l + 1, r - 1, success);
		}else
			if(!valid)
					return 0;
	}
	
	int pivot = search_pivot(l, r);
		if(pivot == -1) {
			*success = false;
			printf("No found pivot.\n");
			return 0;
		}
		Log("the pivot from %d to %d at %d", l, r, pivot);

		int res = 0;
		int right_sub_expr = eval(pivot + 1, r, success);
		if(! *success) return 0;

		if(tokens[pivot].type == TK_DEREF){
			 res = paddr_read((paddr_t)right_sub_expr, 4);
		}else{
			int left_sub_expr = eval(l, pivot - 1, success);
			if(! *success) return 0;
		
			switch(tokens[pivot].type){
				case '+': res = left_sub_expr + right_sub_expr; break;
				case '-': res = left_sub_expr - right_sub_expr; break;
				case '*': res = left_sub_expr * right_sub_expr; break;
				case '/': 
						if(right_sub_expr == 0){
								*success = false;
								printf("0 is divided at token %d.\n", pivot);
								return 0;
						}
						res = left_sub_expr / right_sub_expr;
						break;
				case TK_EQ: res = left_sub_expr == right_sub_expr; break;
				case TK_NEQ: res = left_sub_expr != right_sub_expr; break;
				case TK_AND: res = left_sub_expr && right_sub_expr; break; 
				default:
					printf("Unknown pivot of experssion. pivot: %c.\n", tokens[pivot].type);
			}
		}
		Log("subexperssion from token %d to token %d is equal to %d", l, r, res); 
		return res;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  int res = eval(0, nr_token - 1, success);
	
  return (word_t)res;
}
