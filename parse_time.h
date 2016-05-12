#include "vmplot.h"



extern char* time_format_hints[];
extern size_t time_format_hint_cnt;

extern unsigned int get_format_for_time(const char *);
time_t convert_time_to_epoch(const char *, int, const char *);
