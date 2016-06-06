#define _XOPEN_SOURCE

#include "parse_time.h"

/* list of all the time format we look for 
   NOTE: The first best match will be returned
         so we should make sure we have the longest
         string first and shortest last (descending order)
   Reference: Time Formats are from time.h(3)
 */
char* time_format_hints[] = {
  /* 0 */
  "%Y-%m-%d %H:%M:%S %p",       /* 2016-05-11 09:11:28 AM */
  "%Y-%m-%d %H:%M:%S",
  "%Y-%m-%d",
  "%Y %m %d",
  "%H:%M:%S %p",
  /* 5 */
  "%H:%M:%S",
  "%s",                         /* epoch, 1462983088 */
};

/* number of elems in array is, (sizeof array) / (size of each element)  */
size_t time_format_hint_cnt = sizeof(time_format_hints) / sizeof(char*);

void get_format_for_time_ptr(const char *time_str, char **str) {
  int idx = -1;
  idx = get_format_for_time(time_str);
  if (idx == ~0U) {
    *str = NULL;
    return;
  } else {
    *str = (char *)malloc(sizeof(char) * strlen(time_format_hints[idx]) + 1); /* space of \0 */
    strcpy(*str, time_format_hints[idx]);
  }

  return;
}

/* Tries to find the format for a given time string
   Args: String containing Time
   Returns: 
    - location of the time format in the array
    - ~0U if no match
 */
unsigned int get_format_for_time(const char *time_str) {
  struct tm tm;                 /* time struct */
  unsigned int i;
  char *status = NULL;          /* result of strptime */
  unsigned int retcode = ~0U;       /* ~0U index, we didn't match */
  
  /* clear the tm struct */
  memset(&tm, 0, sizeof(struct tm));

  /* for each element in the time_format_hints */
  for (i = 0; i < time_format_hint_cnt; i++) {
    /* check whether we have a match */
    status = (char *)strptime(time_str, time_format_hints[i], &tm);
    if (status == NULL) {       /* "MISS" time format didn't match */
      continue;
    } else if (*status != '\0' ) { /* "PARTIAL HIT", we should have parsed the complete string not half way through */
      continue;
    } else {                    /* "HIT" , we can return now */
      retcode = i;
      break;
    }
  }
  // retcode has the index
  return retcode;
}

/* Convert a given time to Epoch
   Args:
    - string containing time
    - index to time_format_hints (will be used only if time_format == NULL)
    - time format (if NULL, then set index)
   Returns:
    - epoch in seconds (> -1)
    - -1 for failure
   ERROR: will set vmplot_errno and vmplot_errstr on error
 */
time_t convert_time_to_epoch(const char *time_str, int index, const char *time_fmt) {
  time_t epoch = -1;
  struct tm tm;
  char *time_format = NULL;     /* ptr to time format */
  char *status = NULL;          /* status of strptime */
  /* clear the tm struct */
  memset(&tm, 0, sizeof(struct tm));

  /* clear error */
  clr_error();
  
  /* we look into hint only if time_fmt == NULL */
  if ((index < 0 || index >= time_format_hint_cnt) && time_fmt == NULL) {
    set_error(E_VM_RANGE, "Out of Bound Index");
    return epoch;
  }

  /* if time_fmt is given, use it */
  if (time_fmt)
    time_format = (char *)time_fmt;
  else                          /* else use the format given at index */
    time_format = time_format_hints[index];

  /* parse the given time using the time format  */
  status = (char *)strptime(time_str, time_format, &tm);
  /* if parsing fails, return early */
  if (status == NULL) {
    set_error(E_VM_PARSETIME, "Cannot Parse Time");
    return epoch;
  }

  /* tm_isdst field informs mktime() whether or not daylight saving time (DST) is in effect 
     for the time supplied in the tm structure: 
     - <0 (a positive value) means DST is in effect; 
     -  0 means that DST is not in effect; 
     - >0 (a negative value means) that mktime() should (use timezone information and system databases to) 
       attempt to determine whether DST is in effect at the specified time. 
   */
  tm.tm_isdst = -1;

  /* convert content in tm to epoch */
  epoch = mktime(&tm);
  
  if (epoch == -1) {            /* mktime failed */
    set_error(E_VM_TIMECONV, "Convert to Epoch Failed");
    return epoch;
  }

  /* return the value, we have seen no error */
  return epoch;
}

/* Converts the given epoch to Time string
   Args:
    - epoch
    - index to time_format_hints (will be used only if time_format == NULL)
    - time format (if NULL, then set index)
    - pointer to string where the time will be stored (callee should allocate the memory)
    - size of the string where time will be store
   Returns:
    - FAILURE
    - SUCCESS
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int convert_epoch_to_time(time_t epoch, int index, const char *time_fmt, char *time_str, size_t time_str_sz) {
  int retcode = FAILURE;        /* return code */
  char *time_format = NULL;     /* ptr to time format */
  size_t status = -1;           /* strftime result */

  struct tm *tm;

  /* clear error codes */
  clr_error();

  if((index < 0 && index >= time_format_hint_cnt) && time_fmt == NULL) {
    set_error(E_VM_RANGE, "Out of Bound Index");
    return retcode;
  }

  /* if time_fmt is given, use it */
  if (time_fmt)
    time_format = (char *)time_fmt;
  else                          /* else use the format given at index */
    time_format = time_format_hints[index];
  
  /* from epoch populate tm struct */
  tm = (struct tm*)localtime(&epoch);
  if (tm == NULL) {
    set_error(E_VM_TIMECONV, "Populating tm from epoch Failed ");
    return retcode;
  }
  
  /* convert tm struct to time */
  status = strftime(time_str, time_str_sz, time_format, tm);
  if (status == 0) {            /* actually == 0 is not an error, %p returns empty as per strftime(3) */
    set_error(E_VM_TIMEFMT, "Time Format Failed");
    return retcode;
  } else {
    retcode = SUCCESS;                /* formatting succeeded */
  }

  /* return the return code */
  return retcode;
}

