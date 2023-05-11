/* copyright(C) 2003 H.Kawai (under KL-01). */

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

unsigned long strtoul0(const char **ps, int base, char *errflag);

typedef unsigned char UCHAR;
typedef unsigned int UINT;

static UCHAR *setdec(UCHAR *s, UINT ui)
{
	do {
		*--s = (ui % 10) + '0';
	} while (ui /= 10);
	return s;
}

int vsprintf(char *s, const char *format, va_list arg)
{
	UCHAR c, *t = (UCHAR *)s, *p, flag_left, flag_zero /* , flag_sign, flag_space */;
	UCHAR temp[32] /* �����p */, *q;
	temp[31] = '\0';
	int field_min, field_max, i;
	union {
		long lng;
		UINT uint;
	} l;
	static char hextable_X[16] = "0123456789ABCDEF";
	static char hextable_x[16] = "0123456789abcdef";
	for (;;) {
		c = *format++;
		if (c != '%') {
put1char:
			*t++ = c;
			if (c)
				continue;
			return t - (UCHAR *) s - 1;
		}
		flag_left = flag_zero = /* flag_sign = flag_space = flag_another = */ 0;
		c = *format++;
		for (;;) {
			if (c == '-')
				flag_left = 1;
			else if (c == '0')
				flag_zero = 1;
			else
				break;
			c = *format++;
		}
		field_min = 0;
		if ('1' <= c && c <= '9') {
			format--;
			field_min = (int) strtoul0(&format, 10, (char *)&c);
			c = *format++;
		} else if (c == '*') {
			field_min = va_arg(arg, int);
			c = *format++;
		}
		field_max = INT_MAX;
		if (c == '.') {
			c = *format++;
			if ('1' <= c && c <= '9') {
				format--;
				field_min = (int) strtoul0(&format, 10, (char *)&c);
				c = *format++;
			} else if (c == '*') {
				field_max = va_arg(arg, int);
				c = *format++;
			}
		}
		if (c == 's') {
			if (field_max != INT_MAX)
				goto mikan;
			p = (UCHAR *)va_arg(arg, char *);
			l.uint = strlen((char *)p);
			if (*p) {
				c = ' ';
copy_p2t:
				if (flag_left == 0) {
					while (l.lng < field_min) {
						*t++ = c;
						field_min--;
					}
				}
				do {
					*t++ = *p++;
				} while (*p);
			}
			while (l.lng < field_min) {
				*t++ = ' ';
				field_min--;
			}
			continue;
		}
		if (c == 'l') {
			c = *format++;
			if (c != 'd' && c != 'x' && c != 'u') {
				format--;
				goto mikan;
			}
		}
		if (c == 'u') {
			l.lng = va_arg(arg, UINT);
			goto printf_u;
		}
		if (c == 'd') {
printf_d:
			l.lng = va_arg(arg, long);
			if (l.lng < 0) {
				*t++ = '-';
				l.lng = - l.lng;
				field_min--;
			}
printf_u:
			if (field_max != INT_MAX)
				goto mikan;
			if (field_min <= 0)
				field_min = 1;
			p = setdec(&temp[31], l.lng);
printf_x2:
			c = ' ';
			l.lng = &temp[31] - p;
			if (flag_zero)
				c = '0';
			goto copy_p2t;
		}
		if (c == 'i')
			goto printf_d;
		if (c == '%')
			goto put1char;
		if (c == 'x') {
			q = (UCHAR *)hextable_x;
printf_x:
			l.lng = va_arg(arg, UINT);
			p = &temp[31];
			do {
				*--p = q[l.lng & 0x0f];
			} while (l.uint >>= 4);
			goto printf_x2;
		}
		if (c == 'X') {
			q = (UCHAR *)hextable_X;
			goto printf_x;
		}
		if (c == 'p') {
			i = (int) va_arg(arg, void *);
			p = &temp[31];
			for (l.lng = 0; l.lng < 8; l.lng++) {
				*--p = hextable_X[i & 0x0f];
				i >>= 4;
			}
			goto copy_p2t;
		}
		if (c == 'o') {
			l.lng = va_arg(arg, UINT);
			p = &temp[31];
			do {
				*--p = hextable_x[l.lng & 0x07];
			} while (l.uint >>= 3);
			goto printf_x2;
		}
		if (c == 'f') {
			if (field_max == INT_MAX)
				field_max = 6;
			/* for ese */
			if (field_min < field_max + 2)
				field_min = field_max + 2;
			do {
				*t++ = '#';
			} while (--field_min);
			continue;
		}
mikan:
		for(;;);
	}
}
