#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int _vsnprintf(char *out, size_t n, const char *fmt, va_list ap, char* remain){
	size_t i = 0;
	char *tail, buf[128];

  while (*fmt && i < n){
		if(*fmt++ != '%'){
			out[i++] = *(fmt - 1);
			out[i] = '\0';
			continue;
		}

		switch (*fmt++) {
			case 's': {             /* string */
				tail = va_arg(ap, char *);
				break;
			}
			case 'd':{              /* int */
				int d = va_arg(ap, int);
				buf[0] = d < 0 ? '-' : '\0';
				buf[11] = '\0';
				int j = 10;
				while(d != 0){ 
					buf[j--] = d % 10 + '0';
					d /= 10;
				}

				if (j == 10) {
					buf[j] = '0';
				}else{
					if (buf[0] == '-')
						buf[j] = '-';
					else
						++j;
				}
		
				tail = buf + j;
        break;
			}
			case 'c': {             /* char */
         /* need a cast here since va_arg only
            takes fully promoted types */
          char c = (char) va_arg(ap, int);
					buf[0] = c;
					buf[1] = '\0';
					tail = buf;
					break;
			}
			case 'p': {
			//		void *p = (void *) va_arg(ap, void *);	
					tail = buf;
					break;
			}
			case '%':
					buf[0] = '%';
					buf[1] = '\0';
					tail = buf;
					break;
			default:
				 return -1;
     }
		
		strncat(out + i, tail, n - i - 1);
		if (n - i - 1 > strlen(tail))
			i += strlen(tail);
		else {
			if (remain)
				strcpy(remain, tail + n - i - 1);
			i = n;
		}
	}
	out[i] = '\0';
	
  return i;
}

int printf(const char *fmt, ...) {
	char buf[1024], remain[128];
	int changed = 0;
  va_list ap;
  va_start(ap, fmt);
	do {
		buf[0] = '\0';
		remain[0] = '\0';
		changed += _vsnprintf(buf, 1024, fmt, ap, remain);
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
