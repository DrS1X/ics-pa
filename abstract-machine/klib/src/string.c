#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	if(!s) return 0;
	int len = 0;
	for(; *s != '\0'; ++s, ++len);
  return len;
}

char *strcpy(char* dst,const char* src) {
	char *s = dst;
	const char *i = src;
	for(; *i != '\0'; ++s, ++i)
		*s = *i;
	*s = '\0';
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	char *d = dst;
	const char *s = src;
	for(; n > 0 && *s != '\0'; --n, ++d, ++s)
		*d = *s;
	*d = '\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
	if(!dst || !src)	return NULL;
	char* p = dst;
	p += strlen(dst);
	for(; *src != '\0'; ++p, ++src)
		*p = *src;
	*p = '\0';
  return dst;
}

char* strncat(char* dst, const char* src, size_t n) {
	if(!dst || !src)	return NULL;
	char* p = dst + strlen(dst);
	for(; n > 0 && *src != '\0'; --n)
		*p++ = *src++;
	*p = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
	if(!s1 || !s2)	return 0;
	int res;
	while(*s1 != '\0' && *s2 != '\0'){
		res = *s1 - *s2;
		if(res != 0)
				return res;
		++s1;
		++s2;
	}
	res = *s1 - *s2;
	return res;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	while(n > 0 && *s1 - *s2 == 0 && *s1 != '\0' && *s2 != '\0'){
		++s1;
		++s2;
		--n;
	}
	int res = *s1 - *s2;
  return res;
}

void* memset(void* v,int c,size_t n) {
	char *p = (char *)v;
	for(; n > 0; --n)
		*p++ = c;
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
	if(n == 0) return NULL;
	uint8_t *temp = (uint8_t *)malloc(n * sizeof(uint8_t)); 
	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;
	if(!temp)
		return NULL;
	for(int i = 0; i < n; ++i)
		temp[i] = s[i];
	for(int i = 0; i < n; ++i)
		d[i] = temp[i];
	free(temp);
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
	if(out == in)  return NULL;
	uint8_t *o = (uint8_t *)out;
	uint8_t *i = (uint8_t *)in;
	for(; n > 0; --n, ++o, ++i)
		*o = *i;
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	uint8_t *p1 = (uint8_t *)s1;
	uint8_t *p2 = (uint8_t *)s2;
	while(n > 0 && *p1 - *p2 == 0){
		++p1;
		++p2;
		--n;
	}
	int res = *p1 - *p2;
  return res;
}

#endif
