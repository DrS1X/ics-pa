#include <NDL.h>
#include <SDL.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
	TODO("SDL_OpenAudio");
  return 0;
}

void SDL_CloseAudio() {
	TODO("SDL_CloseAudio");
}

void SDL_PauseAudio(int pause_on) {
	TODO("SDL_PauseAudio");
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
	TODO("SDL_LoadWAV");
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
