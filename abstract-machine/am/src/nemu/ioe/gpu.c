#include <am.h>
#include <nemu.h>
#include <klib.h>

#define SYNC_ADDR (VGACTL_ADDR + sizeof(uint32_t))

static inline int min(int x, int y) { return (x < y) ? x : y; }

void __am_gpu_init() {
	/*
	uint32_t vgactl = inl(VGACTL_ADDR);
  int i;
  int w = vgactl >> 16;
	int h = vgactl & 0xFFFF;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
	outl(SYNC_ADDR, 1);
	*/
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t vgactl = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = vgactl >> 16, .height = vgactl & 0xFFFF,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	uint32_t vgactl = inl(VGACTL_ADDR);
	int screen_w = vgactl >> 16, screen_h = vgactl & 0xFFFF;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  uint32_t *pixels = ctl->pixels;
  int cp_bytes = sizeof(uint32_t) * min(w, screen_w - x);
  for (int j = 0; j < h && y + j < screen_h; j ++) {
    memcpy((void *)(FB_ADDR + sizeof(uint32_t) * ((y + j) * screen_w + x)), pixels, cp_bytes);
    pixels += w;
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
