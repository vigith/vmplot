#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ERRORS */
#define ERANGE     1          /* out of bound array (<0 or >length) */
#define EPARSETIME 2          /* cannot parse time */
#define ETIMECONV  3          /* cannot do time conversion */
#define ETIMEFMT   4          /* time formatting failed */

/* any system that returns error, should call clr_error before processing */
#define clr_error() do { vmplot_errno = 0; vmplot_errstr[0] = '\0'; } while (0);
#define set_error(_errno, _errstr) do { vmplot_errno = _errno; strcpy(vmplot_errstr, _errstr); } while (0)

int vmplot_errno;
char vmplot_errstr[128];
