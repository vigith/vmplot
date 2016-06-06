#include "vmplot.h"

int base = 10;

/* tests whether the given value is a long
   Args: - value
   Returns:
    - FAILURE
    - SUCCESS
*/
int islong(const char *value) {
  long tmp;
  return string_to_long(value, &tmp);
}

/* tests whether the given value is a long
   Args: - value
   Returns:
    - FAILURE
    - SUCCESS
*/
int isfloat(const char *value) {
  float tmp;
  return string_to_float(value, &tmp);
}

/* tests whether the given value is a time
   Args: - value
   Returns:
    - FAILURE
    - SUCCESS
*/
int istime(const char *value, const char *fmt) {
  int retcode = FAILURE;
  unsigned int status;
  /* if fmt is NULL, we will try to get a format */
  if (fmt == NULL) {
    status = get_format_for_time(value); /* try to get a matching format */
    if (status == ~0U) {                 /* oops, we didn't get one */
      return retcode;
    } else {
      retcode = SUCCESS;
    }
  } else {                      /* we have given a format */
    if (convert_time_to_epoch(value, -1, fmt) == -1) {
      return retcode;
    } else {
      retcode = SUCCESS;
    }
  }
  return retcode;
}

/* Converts the given value to a float
   Args:
    - value to be converted
    - pointer to store the result
   Returns:
    - FAILURE
    - SUCCESS
*/
int string_to_float(const char *value, float *number) {
  int status = FAILURE;
  char *next;                   /* point to the next char after processed by strof */

  /* convert to float */
  *number = strtof(value, &next);
  
  if (*next != '\0') {          /* partial conversion */
    return status;
  } else if (next == value) {   /* no conversion */
    return status;              
  } else if (number == 0 && errno == ERANGE) { /* out of range */
    return status;
  } else {
    status = SUCCESS;
  }
  
  return status;
}


/* Converts the given value to long
   Args:
    - value to be converted
    - pointer to store the result
   Returns:
    - FAILURE
    - SUCCESS
*/
int string_to_long(const char *value, long *number) {
  int status = FAILURE;
  char *next;                   /* point to the next char after processed by strtol */

  /* convert to long */
  *number = strtol(value, &next, base);
  
  if (*next != '\0') {          /* partial conversion */
    return status;
  } else if (next == value) {   /* no conversion */
    return status;              
  } else if (number == 0 && errno == ERANGE) { /* out of range */
    return status;
  } else {
    status = SUCCESS;
  }
  
  return status;
}
