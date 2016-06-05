/* process data that is provided to plot the grapht */

#include "process_data.h"

long *data2axis;

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
      d_xdown_cnt++;
    } else if (*tokenptr == 'T') { /* X_TOP */
      data2axis[field++] |= (X_TOP | (xt << 8));
      // xt <<= 1; // XXX: we have single array, no point in incrementing, so we will always use the last defined X_DOWN
      d_xtop_cnt++;
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
      yl <<= 1;                    /* move the next index Y_LEFT */
      d_yleft_cnt++;
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
      d_yright_cnt++;
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


/* Sets the header or name of the axis in info struct
   Args: 
    - Array of character strings
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
   XXX: name/header cannot be more more than X bytes (check info 
        struct in store.h)
*/
int set_store_axis_header(const char **fields) {
  int i = 0;
  char _errstr[128];
  clr_error();
  /* for each opt_field, take the str in **fields and set the name in info struct */
  for (i = 0; i < opt_fields; i++) {
    if (data2axis[i] & X_DOWN) {
      strcpy(st->x_down->xinfo.name, fields[i]);
    } else if (data2axis[i] & X_TOP) {
      strcpy(st->x_top->xinfo.name, fields[i]);
    } else if (data2axis[i] & Y_LEFT) {
      /* we know that data2axis[i] >> 8 should be a +ve number > 0 (--1 to make it an array index)  */
      strcpy(st->y_left_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.name, fields[i]);
    } else if (data2axis[i] & Y_RIGHT) {
      strcpy(st->y_right_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.name, fields[i]);
    } else {
      sprintf(_errstr, "wrong axis set at data2axis[%d]", i);
      set_error(E_VM_WRONGVAL, _errstr);
    }
  }

  return;
}

// test main
int main(void) {
  int status;
  int i;
  const char *header[] = { "foo", "bar", "moo" };
  // create space for data2axis
  data2axis = (long *)malloc(sizeof(long) * opt_fields);
  bzero(data2axis, sizeof(long) * opt_fields);
  // set axis
  status = set_data2axis();
  
  // init store
  status = init_store();
  if (status == FAILURE) {
    printf("ERROR: %s\n", vmplot_errstr);
    return 1;
  }
  // set store_axis_header
  set_store_axis_header(header);
  dump_store();
  //  yaxis **j = st->y_left_arr + 0;
  //printf("%s %p %p %s \n",(*j)->yinfo.name, j, &st->y_left_arr[0], st->y_left_arr[0]->yinfo.name );
  // free
  free(data2axis);
  /* destroy the store */
  destroy_store();
  return 0;
}

