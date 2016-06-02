/* process data that is provided to plot the grapht */

#include "process_data.h"

/* set the values in data2axis int array using the values
   in opt_data2axis. 
   Args: None
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int set_data2axis(void) {
  /* initial all the value to 1 */
  int xd = 1,                   /* X_DOWN */
    yl = 1,                     /* Y_LEFT */
    yr = 1,                     /* Y_RIGHT */
    xt = 1;                     /* X_TOP */
  
  int retcode = FAILURE;        /* return status */
  char *saveptr;                /* reentrant tokenizer ; between axes ..D;L.. */
  char *tokenptr;               /* pointer to next token */
  char *tmpstr;                 /* tmp string for strtok */
  int field = 0;                /* current field worked upon in data2axis */
  
  /* clear error */
  clr_error();


  /* we need to make sure opt_data2axis has some valid str */
  if (strlen(opt_data2axis) == 0) {
    set_error(E_VM_WRONGVAL, "opt_data2axis cannot be empty");
    return retcode;
  }
  
  /* if we don't have ; */
  if (strchr(opt_data2axis, ';') == NULL) {
    set_error(E_VM_WRONGVAL, "opt_data2axis Missing ;");
    return retcode;
  }
    
  /* free this memory, strdup malloc's */
  tmpstr = strdup(opt_data2axis);

  /* tokenize the string and this is never NULL
     XXX: we will always have something at X and Y axis,
     in case user asks to plot an 1 variable graph then
     x-axis will be just a counter.
   */
  tokenptr = strtok_r(tmpstr, ";", &saveptr);
  do {
    if (*tokenptr == 'D') {                       /* X_DOWN */
      data2axis[field++] |= (X_DOWN | (xd << 8)); /* we don't use the MSB actually */
      // xd <<= 1; // XXX: we have single array, no point in incrementing, so we will always use the last defined X_DOWN
    } else if (*tokenptr == 'T') { /* X_TOP */
      data2axis[field++] |= (X_TOP | (xt << 8));
      // xt <<= 1; // XXX: we have single array, no point in incrementing, so we will always use the last defined X_DOWN
    } else if (*tokenptr == 'L') { /* Y_LEFT */
      /* we assume we will get :D or :T after this */
      if (tokenptr+1 != NULL && *(tokenptr+1) == ':' && tokenptr+2 != NULL) {
        if (*(tokenptr+2) == 'D') { /* X_DOWN w.r.t Y_LEFT */
          data2axis[field++] |= (Y_LEFT | (yl << 8) | (X_DOWN << 24)); /* the MSB contains which index we need to add the entry to */
        } else if (*(tokenptr+2) == 'T') { /* X_TOP w.r.t Y_LEFT */
          data2axis[field++] |= (Y_LEFT | (yl << 8) | (X_TOP << 24)); /* the MSB contains which index we need to add the entry to */
        } else {
          set_error(E_VM_WRONGVAL, "opt_data2axis expected D/T");
          break;
        }
      } else {
        set_error(E_VM_WRONGVAL, "opt_data2axis expected : D/T");
        break;
      }
      yl <<= 1;               /* move the next index Y_LEFT */
    } else if (*tokenptr == 'R') { /* Y_RIGHT */
      /* we assume we will get :D or :T after this */
      if (tokenptr+1 != NULL && *(tokenptr+1) == ':' && tokenptr+2 != NULL) {
        if (*(tokenptr+2) == 'D') { /* X_DOWN w.r.t Y_RIGHT */
          data2axis[field++] |= (Y_RIGHT | (yr << 8) | (X_DOWN << 24)); /* the MSB contains which index we need to add the entry to */
        } else if (*(tokenptr+2) == 'T') { /* X_TOP w.r.t Y_RIGHT */
          data2axis[field++] |= (Y_RIGHT | (yr << 8) | (X_TOP << 24)); /* the MSB contains which index we need to add the entry to */
        } else {
          set_error(E_VM_WRONGVAL, "opt_data2axis expected D/T");
          break;
        }
      } else {
        set_error(E_VM_WRONGVAL, "opt_data2axis expected : D/T");
        break;
      }
      yr <<= 1;                 /* move the next index for Y_RIGHT */
    } else {
      set_error(E_VM_WRONGVAL, "opt_data2axis exptected D/T/L/R");
      break;
    }
  } while ((tokenptr = strtok_r(NULL, ";", &saveptr)) != NULL) ;

  if (vmplot_errno == 0)
    retcode = SUCCESS;
  
  /* free the tmpstr allocation */
  free(tmpstr);

  return retcode;
}


/*
int main(void) {
  int status;
  int i;
  // create space for data2axis
  data2axis = (int *)malloc(sizeof(int) * opt_fields);
  bzero(data2axis, sizeof(int) * opt_fields);
  status = set_data2axis();
  for(i=0; i<opt_fields; i++) {
    printf("%d\n", data2axis[i]);
  }
  free(data2axis);
  return 0;
}
*/
