#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* STATUS Codes */
#define SUCCESS 1
#define FAILURE 0

/* Array Increment Size */
#define VM_ARRAY_SZ    256      /* default array size to store data points */
/* Field Format Sep btwn Type and Fmt */
#define VM_FLD_FMT_SEP '#'
/* padding on each side of the graph */
#define VM_PADDING 1

/* ERRORS */
#define E_VM_RANGE      1          /* out of bound array (<0 or >length) */
#define E_VM_PARSETIME  2          /* cannot parse time */
#define E_VM_TIMECONV   3          /* cannot do time conversion */
#define E_VM_TIMEFMT    4          /* time formatting failed */
#define E_VM_MALLOC     5          /* malloc failed */
#define E_VM_WRONGVAL   6          /* wrong value in the variable */
#define E_VM_NOMANSLAND 7          /* No Man's Land, shouldn't have ever reached here */
#define E_VM_EXTFNCTN   8          /* external function failed */

/* any system that sets error code, should call clr_error before processing */
#define clr_error() do { vmplot_errno = 0; vmplot_errstr[0] = '\0'; } while (0)
/* set the error code and error string */
#define set_error(_errno, _errstr) do { vmplot_errno = _errno; strcpy(vmplot_errstr, _errstr); } while (0)

/***********/
/* GLOBALS */
/***********/

/* errors */
int vmplot_errno;
char vmplot_errstr[128];

/* options */
extern int opt_fields;            /* how many plots to make */
extern char *opt_in_field_hints;  /* hint to field type type[: optional fmt] (eg, INT,FLOAT,TIME:fmt) */
extern char *opt_out_field_hints; /* hint to field type type[: optional fmt] (eg, INT,FLOAT,TIME:fmt) */
extern char *opt_data2axis;       /* data to axis mapping */
/* derived values from options */
/* store and process */
extern int d_xdown_y;           /* number of y variables are plotted againx xdown */
extern int d_xtop_y;            /* number of y variables are plotted againx xtop */
extern int d_xdown_cnt;         /* X-down plot count */
extern int d_xtop_cnt;          /* X-top plot count */
extern int d_yleft_cnt;         /* Y-left plot count */
extern int d_yright_cnt;        /* Y-right plot count */
extern int d_axes;              /* axes which are used (derived from opt_data2axis) */
extern int d_cir_buff;          /* use circular buffer, in case of streaming */
/* transformation func args */
extern char *d_in_field_hints[];    /* derived field hints from opt_in_field_hints */
extern char *d_out_field_hints[];    /* derived field hints from opt_in_field_hints */
/* logger */
extern char *d_logloc;        /* log location */
/* how close should points be in graph */
extern int opt_compact_x;
extern int opt_compact_y;
extern int opt_label_vert;      /* no labelling can set this to 0 */
extern int opt_label_horz;      /* no labelling can set this to 0 */
