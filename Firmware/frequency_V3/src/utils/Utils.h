#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h> // Für `char` und `long` Typen

// Declaration of utility functions
char *ultos_recursive(unsigned long val, char *s, unsigned radix, int pos);
char *ltos(long val, char *s, int radix);

#endif // UTILS_H