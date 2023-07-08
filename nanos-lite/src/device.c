#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	char *ch = (char *)buf;
	int i = 0;
	for (; i < len; ++i)
		putch(ch[i]);
  return i;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T event = io_read(AM_INPUT_KEYBRD);
	if (event.keycode == 0) return 0;

	char up_down = event.keydown ? 'd' : 'u';
	const char * key_name = keyname[event.keycode];
	int written = snprintf((char*) buf, len, "k%c %s\n", up_down, key_name);
	assert(written >= 0);
  return written;
}

size_t screen_w, screen_h;

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	AM_GPU_CONFIG_T conf = io_read(AM_GPU_CONFIG);
	screen_w = conf.width;
	screen_h = conf.height;
	int written = snprintf((char*) buf, len, "WIDTH : %d\nHEIGHT : %d", conf.width, conf.height);
	assert(written >= 0);
  return written;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  // AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);
  io_write(AM_GPU_FBDRAW, offset % screen_w, offset / screen_w, (void *)buf, len / sizeof(uint32_t), 1, true); 
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
