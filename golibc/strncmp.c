//*****************************************************************************
// strncmp.c : string function
// 2002/02/04 by Gaku : this is rough sketch
//*****************************************************************************

#include <stddef.h>

//=============================================================================
// compare no more SZ bytes D and S
//=============================================================================
int	strncmp (char *d, const char *s, size_t sz)
{
#if 0
	/* Gaku's code */

	while ( sz-- && '\0' != *d ) {
		if (*d != *s)
			return *d - *s;
		d++;
		s++;
	}
	return *d - *s; /* bug? (if sz == 0) */

#endif

#if 1
	/* Kawai's code */

	while (sz--) {
		if (*d == '\0')
			goto ret;
		if (*d != *s) {
ret:
			return *d - *s;
		}
		d++;
		s++;
	}
	return 0;
#endif

}

int	strncmpi (char *d, const char *s, size_t sz)
{
	int i;
	char d2[sz + 1];
	char s2[sz + 1];
	strncpy(d2, d, sz);
	d2[sz] = 0;
	strncpy(s2, s, sz);
	s2[sz] = 0;
	
#if 0
	/* Gaku's code */
	
	i = 0;
	while ( sz-- && '\0' != *d ) {
		if ('A' <= d2[i] && d2[i] <= 'Z') d2[i] += 0x20;
		if ('A' <= s2[i] && s2[i] <= 'Z') s2[i] += 0x20;
		if (d2[i] != s2[i])
			return d2[i] - s2[i];
		i++;
	}
	return d2[i] - s2[i]; /* bug? (if sz == 0) */
	
#endif
	
#if 1
	/* Kawai & Kenta's code */
	
	while (sz--) {
		if (d2[i] == '\0')
			goto ret;
		if ('A' <= d2[i] && d2[i] <= 'Z') d2[i] += 0x20;
		if ('A' <= s2[i] && s2[i] <= 'Z') s2[i] += 0x20;
		if (d2[i] != s2[i]) {
		ret:
			return d2[i] - s2[i];
		}
		i++;
	}
	return 0;
#endif
	
}
