#include "watchpoint.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
		(wp_pool[i].expr)[0] = '\0';
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(char *wp_expr){
	if(free_ == NULL){
			printf("No free watchpoint in the pool.\n");
			return NULL;
	}

	bool success;
  word_t val = expr(wp_expr, &success); 
	if(! success){
			printf("Expression is invalid.\n");
			return NULL;
	}

	WP *p = free_;
	free_ = free_->next;

	p->next = head;
	head = p;

	strcpy(p->expr, wp_expr);
	p->val = val;
	return p;
}

void free_wp(int NO){
	if(head == NULL){
		printf("No watchpoint.\n");
		return;
	}

	WP *wp = NULL;
	for(WP **pnext = &head; *pnext; pnext = &(*pnext)->next){
			if((*pnext)->NO == NO){
					wp = *pnext;
					*pnext = wp->next;
					break;
			}
	}

	if(!wp){
			printf("No matching watchpoint.\n");
			return;
	}

	wp->next = free_->next;
	free_ = wp;

	(wp->expr)[0] = '\0';
}

bool check_wp(){
		bool change = false;
		for(WP *p = head; p; p = p->next){
				bool success;
				word_t new_val = expr(p->expr, &success);
				if(new_val != p->val){
						printf("Watchpoint %d: %s\nOld value = %u\nNew value = %u\n",
										p->NO, p->expr, p->val, new_val);
				}
				p->val = new_val;
				change = true;
		}
		return change;
}

void display_wp(){
		printf("%-5s %-10s %s\n", "NO", "value", "expression");
		for(WP *p = head; p; p = p->next){
				printf("%-5d %-10u %s\n", p->NO, p->val, p->expr);
		}
}

