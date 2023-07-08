#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <NDL.h>

int main() {
  NDL_Init(0);
	uint32_t prev = NDL_GetTicks(), cur;
	for(;;) {
		cur = NDL_GetTicks();

		if (cur - prev > 500) {
			prev = cur;
			printf("msec: %u\n", cur);
		}
	}
	return 0;
}
