#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* STATUS Codes */
#define SUCCESS 1
#define FAILURE 0

/* ERRORS */
#define E_VM_RANGE     1          /* out of bound array (<0 or >length) */
#define E_VM_PARSETIME 2          /* cannot parse time */
#define E_VM_TIMECONV  3          /* cannot do time conversion */
#define E_VM_TIMEFMT   4          /* time formatting failed */

/* any system that sets error code, should call clr_error before processing */
#define clr_error() do { vmplot_errno = 0; vmplot_errstr[0] = '\0'; } while (0)
/* set the error code and error string */
#define set_error(_errno, _errstr) do { vmplot_errno = _errno; strcpy(vmplot_errstr, _errstr); } while (0)

int vmplot_errno;
char vmplot_errstr[128];
