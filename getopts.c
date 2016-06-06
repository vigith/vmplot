#include "vmplot.h"

/* TODO: really read the opts, for now just hardcode it to ease the development */

int opt_fields = 3;
char *opt_in_field_hints = "TIME:%Y-%m-%d %H:%M:%S %p,LONG,FLOAT";
char *opt_out_field_hints = "TIME:%Y-%m-%d %H:%M:%S %p,LONG,FLOAT";
/* TODO: if empty set it based on opt_fields (first is X_DOWN and rest are Y_LEFT, for Y_LEFT mention X_DOWN as axis) */
char *opt_data2axis = "D;L:D;L:D";

int d_axes   = 0;               /* TODO: use in set_label_axis */
/* TODO: derive from opt_data2axis */
int d_xdown_y = 3;
int d_xtop_y = 0;

/* derived by set_data2axis */
int d_xdown_cnt = 0;            /* how many x down */
int d_xtop_cnt = 0;             /* how many x top */
int d_yleft_cnt = 0;            /* how many y left */
int d_yright_cnt = 0;           /* how many y right */
char *d_in_field_hints[] = {NULL, NULL, NULL};//"TIME:%Y-%m-%d %H:%M:%S %p","LONG","FLOAT"};
char *d_out_field_hints[] = {"TIME:%Y-%m-%d %H:%M:%S %p","LONG","FLOAT"};
