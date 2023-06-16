#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define BUF_SIZE 1024
#define REM_SIZE 128

struct number {
	uint32_t val;
	uint8_t base;
	char prefix[3];
	size_t width;
	char padding;
};

char* num2str (struct number *num, char* buf) {
	/*putch('!');
	putch(num->width + '0');
	putch('\n');
	*/
	buf[BUF_SIZE - 1] = '\0';
	int i = BUF_SIZE - 2;
	while (num->val != 0){ 
		uint8_t mod = num->val % num->base;
		buf[i--] = mod >= 10 ? mod - 10 + 'a' : mod + '0';
		num->val /= num->base;
	}

	// zero
	if (i == BUF_SIZE - 2) {
		buf[i--] = '0';
	} else {
		// prefix
		if (num->prefix[0] != '\0') {
			int k = strlen(num->prefix) - 1;
			for (; k >= 0; --k) {
				buf[i--] = num->prefix[k];
			}
		}
	}

	char * str = buf + i + 1;
	// align to fix width
	if (num->width != 0) {
		int diff = num->width - strlen(str);
		if (diff > 0) {
			str = buf + BUF_SIZE - num->width - 1;
			memset(str, num->padding, diff);
		}
	}

	return str;
}

enum {PRT_FMT, PRT_NONE, PRT_ESCAPE, PRT_FMT_W};

int _vsnprintf(char *out, size_t n, const char *fmt, va_list ap, char* remain){
	out[0] = '\0';
	size_t i = 0;
	char *tail, buf[BUF_SIZE];
	int state = PRT_NONE; 
	struct number num;

	while (*fmt && i < n - 1){
		char ch = *fmt++;
		if (state == PRT_NONE){
			switch (ch) {
				case '%':
					state = PRT_FMT;
					num.val = 0;
					num.base = 10;
					num.prefix[0] = '\0';
					num.width = 0;
					num.padding = ' ';
					break;
				case '\\':
					state = PRT_ESCAPE;
					break;
				default:
					out[i++] = ch;
					out[i] = '\0';
			}
		}
		else if (state == PRT_ESCAPE) {
			switch (ch) {
				case 'n':
					out[i++] = '\n';
					break;
				default:
					out[i++] = ch;
			}
			out[i] = '\0';
			state = PRT_NONE;
		}
		else if (state == PRT_FMT_W) {
			if (ch >= '0' && ch <= '9') {
				num.width = num.width * 10 + ch - '0';
			} else {
				state = PRT_FMT;
				--fmt;
			}
		}
		else if (state == PRT_FMT) {
			tail = NULL;
			if (ch > '0' && ch <= '9') { // '0' is used to pad
				state = PRT_FMT_W;
				num.width = ch - '0';
			} 
			else { 
				switch (ch) {
					case '#':
						num.prefix[0] = '#';
						num.prefix[1] = '\0';
						break;
					case 's':              /* string */
						tail = va_arg(ap, char *);
				    state = PRT_NONE;
						break;
					case 'd':{              /* decimal int */
						int d = va_arg(ap, int);
						if (d < 0){
							num.prefix[0] = '-';
							num.prefix[1] = '\0';
							num.val = -d;
						} else
							num.val = d;
						tail = num2str(&num, buf);
				    state = PRT_NONE;
						break;
					}
					case 'x':{              /* hexadecimal int */
						int d = va_arg(ap, int);
						num.base = 16;
						if (num.prefix[0] != '\0') { 
							num.prefix[0] = '0';
							num.prefix[1] = 'x';
							num.prefix[2] = '\0';
						}
						num.val = d;
						tail = num2str(&num, buf);
				    state = PRT_NONE;
						break;
					}
					case 'p': {							/* pointer */
							uintptr_t p = va_arg(ap, uintptr_t);	
							num.base = 16;
							strcpy(num.prefix, "0x");
							num.val = p;
							tail = num2str(&num, buf);
				      state = PRT_NONE;
							break;
					}
					case 'c': {             /* char */
							char c = (char) va_arg(ap, int);
							buf[0] = c;
							buf[1] = '\0';
							tail = buf;
				      state = PRT_NONE;
							break;
					}
					case '%':
							buf[0] = '%';
							buf[1] = '\0';
							tail = buf;
				      state = PRT_NONE;
							break;
					default:
						 num.padding = ch;
				 }
			}

			if (state ==  PRT_NONE && tail) {
				strncat(out, tail, n - i - 1);
				if (n - i - 1 > strlen(tail)) {
					i += strlen(tail);
				} else {
					if (remain)
						strncpy(remain, tail + n - i - 1, REM_SIZE);
					i = n - 1;
					break;
				}
			}	
		} 
	}
	out[i] = '\0';

	return i;
}

int printf(const char *fmt, ...) {
	char buf[BUF_SIZE], remain[REM_SIZE];
	int changed = 0;
  va_list ap;
  va_start(ap, fmt);
	do {
		buf[0] = '\0';
		remain[0] = '\0';
		changed += _vsnprintf(buf, BUF_SIZE, fmt, ap, remain);
		assert(strlen(buf) <= BUF_SIZE);
		assert(strlen(remain) <= REM_SIZE); 
		putstr(buf);
		putstr(remain);
	} while (strlen(remain) != 0);
  va_end(ap);
	return changed;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	return vsnprintf(out, SIZE_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int changed = vsprintf(out, fmt, ap);
  va_end(ap);
	return changed;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int changed = vsnprintf(out, n, fmt, ap);
  va_end(ap);
	return changed;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
		return _vsnprintf(out, n, fmt, ap, NULL);	
}

#endif
