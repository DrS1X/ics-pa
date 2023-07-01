#include <am.h>
#include <riscv32.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
	/*for (int i = 0; i < 32; ++i) 
		printf("x%d: %#x, ", i, c->gpr[i]);
	printf("cause: %#x, ", c->cause);
	printf("status: %#x, ", c->status);
	printf("epc: %#x.\n", c->epc);
*/
  if (user_handler) {
    Event ev = {0};
    switch (c->cause) {
			case 8: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
