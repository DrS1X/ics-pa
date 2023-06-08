#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 32)

void __am_gpu_init() {
	uint32_t w_h = inl(VGACTL_ADDR);
  int i;
  int w = w_h >> 16;
	int h = w_h & 0xFFFF;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
	outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t w_h = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w_h >> 16, .height = w_h & 0xFFFF,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
