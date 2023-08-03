#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];	
	if (NDL_PollEvent(buf, sizeof(buf))) {
		if (buf[0] == 'k') {
			ev->key.type = buf[1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
			uint8_t i;
			for (i = 0; i < sizeof(keyname) / sizeof(char*) && strcmp(buf + 3, keyname[i]) != 0; ++i); 
			assert(i < sizeof(keyname) / sizeof(char*));
			ev->key.keysym.sym = i;
		}

		return 1;
	}
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	while (!SDL_PollEvent(event));
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
