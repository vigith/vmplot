#include "vmplot.h"

/* TODO: really read the opts, for now just hardcode it to ease the development */

int opt_fields = 3;
/* TODO: if empty set it based on opt_fields (first is X_DOWN and rest are Y_LEFT, for Y_LEFT mention X_DOWN as axis) */
char *opt_data2axis = "D;L:D;L:D";

int d_axes   = 0;               /* TODO: use in set_label_axis */
/* TODO: derive from opt_data2axis */
int d_xdown_y = 3;
int d_xtop_y = 0;
int d_yleft_cnt = 3;
int d_yright_cnt = 0;
