#include "vmplot.h"
#include "parse_time.h"

extern int base;                /* base for strtol */

extern int islong(const char *);
extern int isfloat(const char *);
extern int istime(const char *, const char *);
extern int string_to_float(const char *, float *);
extern int string_to_long(const char *, long *);
