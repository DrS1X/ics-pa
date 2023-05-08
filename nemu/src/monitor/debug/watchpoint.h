#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>
#include "expr.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	char expr[1024];
	word_t val;
} WP;

void init_wp_pool();

WP* new_wp(char *wp_expr);

void free_wp(int NO);

bool check_wp();

void display_wp();

#endif
