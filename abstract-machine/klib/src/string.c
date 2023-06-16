#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	if(!s) return 0;
	size_t len = 0;
	for(; s[len] != '\0'; ++len);
  return len;
}

char *strcpy(char* dst,const char* src) {
	size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
	dst[i] = '\0';
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for ( ; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
	 size_t dst_len = strlen(dst);
	 size_t i;
	 for (i = 0 ; src[i] != '\0' ; i++)
			 dst[dst_len + i] = src[i];
	 dst[dst_len + i] = '\0';
	 return dst;
}

char* strncat(char* dst, const char* src, size_t n) {
	 size_t dst_len = strlen(dst);
	 size_t i;
	 for (i = 0 ; i < n && src[i] != '\0' ; i++)
			 dst[dst_len + i] = src[i];
	 dst[dst_len + i] = '\0';
	 return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i = 0;
	for (; s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0'; ++i);
	return s1[i] - s2[i];
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i = 0;
	for (; i < n && s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0'; ++i);
	return i == n ? 0 : s1[i] - s2[i];
}

void* memset(void* v,int c,size_t n) {
	//printf("!3\n");
	char *ch = v;
	for(size_t i = 0; i < n; ++i)
		ch[i] = c;
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
	//printf("!2\n");
	if (n == 0) return NULL;
	uint8_t *temp = (uint8_t *)malloc(n * sizeof(uint8_t)); 
	const uint8_t *s = src;
	uint8_t *d = dst;
	if (!temp)	return NULL;
	for (size_t i = 0; i < n; ++i)
		temp[i] = s[i];
	for (size_t i = 0; i < n; ++i)
		d[i] = temp[i];
	free(temp);
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
	//printf("!1\n");
	char *o = out;
	const char *i = in;
	for(size_t k = 0; k < n; ++k)
		o[k] = i[k];
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	//printf("!0\n");
	const char *p1 = s1;
	const char *p2 = s2;
	for(size_t i = 0; i < n; ++i)
		if (p1[i] != p2[i]) return p1[i] - p2[i];
	return 0;
}

#endif
