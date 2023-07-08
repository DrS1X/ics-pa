#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int offset_w = 0, offset_h = 0;

uint32_t NDL_GetTicks() {
	struct timeval tv;
	int err = gettimeofday(&tv, NULL);
	assert(err == 0);

	uint32_t millisecond = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return millisecond;
}

int NDL_PollEvent(char *buf, int len) {
	int fd = open("/dev/events", O_RDONLY);
	int got = read(fd, buf, len);
	close(fd);
	return got;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
	else {
		fbdev = 5;
		char info[128];
		read(4, info, sizeof(info) - 1);	
		int value_start = 0;
		for (int i = 0;  i < 128 && info[i] != '\0'; ++i) {
			if (info[i] == ':') value_start = i + 2;
			else if (info[i] == '\n') {
				info[i] = '\0';
				screen_w = atoi(&info[value_start]);
			}
		}
		assert(value_start != 0);
		screen_h = atoi(&info[value_start]);
    
		offset_w = (screen_w - *w) / 2;
		offset_h = (screen_h - *h) / 2;

		//printf("NDL_OPEN, screen_w: %d, screen_h: %d\n", screen_w, screen_h);
	}
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	for (int i = 0; i < h; ++i) {
		lseek(fbdev, (y + offset_h + i) * screen_w + x + offset_w, SEEK_SET);
		write(fbdev, pixels, w * sizeof(uint32_t));
		pixels += w;
	}
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
