#include "vmplot.h"

/* Variables */
//extern char* time_format_hints[];
extern size_t time_format_hint_cnt;

/* Functions */
extern void get_format_for_time_ptr(const char *, char **);
extern unsigned int get_format_for_time(const char *);
extern time_t convert_time_to_epoch(const char *, int, const char *);
extern int convert_epoch_to_time(time_t, int, const char *, char *, size_t);
