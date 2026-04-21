#include "Utils.h"

// Implementations of utility functions from original code
char *ultos_recursive(unsigned long val, char *s, unsigned radix, int pos) {
  int c;
  if (val >= radix)
    s = ultos_recursive(val / radix, s, radix, pos + 1);
  c = val % radix;
  c += (c < 10 ? '0' : 'a' - 10);
  *s++ = c;
  // if (pos % 3 == 0) *s++ = ','; // Kommas nur, wenn gewünscht (im Originalcode aktiv)
  return s;
}

char *ltos(long val, char *s, int radix) {
  if (radix < 2 || radix > 36) {
    s[0] = 0;
  } else {
    char *p = s;
    if (radix == 10 && val < 0) {
      val = -val;
      *p++ = '-';
    }
    p = ultos_recursive(val, p, radix, 0) - 1;
    *p = 0;
  }
  return s;
}