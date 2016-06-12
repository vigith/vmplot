#include "store.h"

store *st = NULL;               /* start with an NULL pointer, easy to free */
long *data2axis;
long global_idx;

/*  TODO: write it properly
 */
void set_label_axis(int *axes) {
  *axes |= X_DOWN;
  *axes |= Y_LEFT;
  return;
}

/* stores the given string as a long into the store
   Args: 
    - void * to input_fn_arg
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int store_str_as_long(void *data) {
  input_fn_arg *arg = (input_fn_arg *)data;
  int status;
  long lg;
  
  clr_error();
  /* convert to log */
  status = string_to_long(arg->input_data, &lg);
  /* return if failure */
  if (status == FAILURE) return FAILURE;
  /* store the data */
  *((long *)arg->addr) = lg;

  return SUCCESS;
}

/* stores the given string as a time into the store
   Args: 
    - void * to input_fn_arg
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int store_str_as_float(void *data) {
  input_fn_arg *arg = (input_fn_arg *)data;
  int status;
  float fl;
  
  clr_error();
  /* convert to float */
  status = string_to_float(arg->input_data, &fl);
  /* return if failure */
  if (status == FAILURE) return FAILURE;
  
  /* store the data */
  *((float *)arg->addr) = fl;

  return SUCCESS;
}

/* stores the given string as a time into the store
   Args: 
    - void * to input_fn_arg
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int store_str_as_time(void *data) {
  input_fn_arg *arg = (input_fn_arg *)data;
  time_t tm;
  char **fmt;
  clr_error();
  
  /* we have already found the time format either via CLI hint or using get_format_for_time */
  fmt = (char **)arg->state;     /* get format from state */
  /* convert time string to epoch using the fmt stored in the in_hint_args array */
  tm = convert_time_to_epoch(arg->input_data, -1, fmt[(int)(arg->index)]);
  /* if error, return. vmplot_err* is already set */
  if (tm < 0) return FAILURE;

  /* store the data */
  *((time_t *)arg->addr) = tm;

  return SUCCESS;
}

/* stores the given string into the store
   Args: 
    - void * to input_fn_arg
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
int store_str(char **input, void *state) {
  int i;
  int status;                   /* status of the function call */
  input_fn_arg ifn_arg;         /* input function arg */
  input_fn ifn;                 /* input function */
  char _errstr[128];            /* error string */
  void *addr = NULL;            /* address of the location to store the data */
  datum *addr_d = NULL;         /* address of the datum which points to datum union */
  data_type dt = NOSUCH;        /* data type */
  
  clr_error();
  
  for (i = 0; i < opt_fields; i++) {
    /* get the appropriate transformation func */
    if (data2axis[i] & X_DOWN) {
      ifn = st->x_down->xinfo.t_fns.i_fn;
      addr_d = &st->x_down->row.val[global_idx];
      dt = st->x_down->xinfo.d_type;
    } else if (data2axis[i] & X_TOP) {
      ifn = st->x_top->xinfo.t_fns.i_fn;
      addr_d = &st->x_top->row.val[global_idx];
      dt = st->x_top->xinfo.d_type;
    } else if (data2axis[i] & Y_LEFT) {
      ifn = st->y_left_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.t_fns.i_fn;
      addr_d = &st->y_left_arr[ffsl(data2axis[i] >> 8)-1]->val[global_idx];
      dt = st->y_left_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.d_type;
    } else if (data2axis[i] & Y_RIGHT) {
      ifn = st->y_right_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.t_fns.i_fn;
      dt = st->y_right_arr[ffsl(data2axis[i] >> 8)-1]->yinfo.d_type;
    } else {
      sprintf(_errstr, "wrong axis set at data2axis[%d]", i);
      set_error(E_VM_WRONGVAL, _errstr);
      return FAILURE;
    }

    if (dt == LONG) addr = &addr_d->lg_value;
    else if (dt == FLOAT) addr = &addr_d->fl_value;
    else if (dt == TIME) addr = &addr_d->tm_value;
    else {
      set_error(E_VM_NOMANSLAND, "NML:store_str, dt not set");
      return FAILURE;
    }
        
    /* build the struct for calling the input transformation func */
    ifn_arg.input_data = (char *)input[i];
    ifn_arg.index = i;
    ifn_arg.addr = addr;
    ifn_arg.state = state;
    /* call the function */
    status = ifn((void *)&ifn_arg);
    /* reset */
    addr = NULL;
    addr_d = NULL;
    dt = NOSUCH;
    if (status == FAILURE) {
      return FAILURE;           /* errno and errstr will be propagated from the transformation func */
    }
  }
  
  printf("%d ** %d\n", st->x_down->row.val[global_idx].tm_value, i);
  printf("%d -- %d\n", st->y_left_arr[0]->val[global_idx].lg_value, i);
  printf("%f -- %d\n", st->y_left_arr[1]->val[global_idx].fl_value, i);

  return;
}

/* intialize the Y-Axis
   The fuction will free all the allocated memory that happened
   in this fuction if there is an error.
   Args: 
    - ***yaxis (left or right)
    - number of plots in y-axis
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
static int init_yaxis(yaxis ***_y, int y_cnt) {
  int retcode = SUCCESS;
  int i;
  
  yaxis **y;                     /* local ptr */

  /* clear error */
  clr_error();
  
  /* initiallize the array which points to array of yaxis */
  y = (yaxis **)malloc(sizeof(yaxis *) * y_cnt);
  if (y == NULL) {
    set_error(E_VM_MALLOC, "(malloc) *yaxis Failed");
    retcode = FAILURE;
    goto yaxisfail;
  }
  /* update the axis w.r.t store */
  *_y = y;

  /* for each ptr in the array, intialize the yaxis plot */
  for(i = 0; i < y_cnt; i++) {
    y[i] = (yaxis *)malloc(sizeof(yaxis)); /* now y[i] and (*_y)[i] points to same yaxis */
    if (y[i] == NULL) {
      set_error(E_VM_MALLOC, "(malloc) **yaxis Failed");
      retcode = FAILURE;
      goto yaxisptrfail;
    }
  }

  /* for each y-axis plot, create the space to store points */
  for(i = 0; i < y_cnt; i++) {
    y[i]->val = (datum *)malloc(sizeof(datum) * VM_ARRAY_SZ); /* now y[i]->val and (*_y)[i]->val points to same yaxis datum */
    if (y[i]->val == NULL) {
      set_error(E_VM_MALLOC, "(malloc) **yaxis->val Failed");
      retcode = FAILURE;
      goto yaxisvalfail;
    }
  }
    
  if(retcode == SUCCESS)
    return retcode;

 yaxisvalfail:
  /* decrement first, because i failed, only i-- has valid ptr */
  for(i--; i >= 0; i--) {          /* free the opposite way */
    free(y[i]->val);
  }
  
 yaxisptrfail:
  /* decrement first, because i failed, only i-- has valid ptr */
  for(i--; i >= 0; i--) {          /* free the opposite way */
    free(y[i]);
  }
    
 yaxisfail:
  *_y = NULL;

  return retcode;
}

/* intialize the X-Axis
   The fuction will free all the allocated memory that happened
   in this fuction if there is an error.
   Args: 
    - **xaxis (down or top)
    - number of y axis to be plotted against x-axis
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
*/
static int init_xaxis(xaxis **_x, int x_to_y_cnt) {
  int retcode = SUCCESS;
  /* clear error */
  clr_error();

  xaxis *x;                     /* local ptr */

  /* x axis */
  x = (xaxis *)malloc(sizeof(xaxis));
  if (x == NULL) {
    set_error(E_VM_MALLOC, "(malloc) xaxis Failed");
    retcode = FAILURE;
    goto xaxisfail;
  }
  /* update the axis w.r.t store */
  *_x = x;                      /* write this location in the store */

  /* initialize table */
  /* value */
  x->row.val = (datum *)(malloc(sizeof(datum) * VM_ARRAY_SZ));
  if (x->row.val == NULL) {
    set_error(E_VM_MALLOC, "(malloc) row.val Failed");
    retcode = FAILURE;
    goto rowvalfail;
  }
  /* pointer to y data */
  x->row.index_arr = (int *)(malloc(sizeof(int) * VM_ARRAY_SZ * x_to_y_cnt));
  if (x->row.index_arr == NULL) {
    set_error(E_VM_MALLOC, "(malloc) row.index_arr Failed");
    retcode = FAILURE;
    goto rowarrfail;
  }

  /* return if all is good */
  if (retcode == SUCCESS)
    return retcode;

  /* failures */
 rowarrfail:
  free(x->row.val);
    
 rowvalfail:
  free(x);

 xaxisfail:
  *_x = NULL;                   /* we started off with NULL */
  
  return retcode;
}

/* intialize the all the Axis
   Args: None
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will propogate the vmplot_errno and vmplot_errstr from the 
          functions it calls.
*/
static int init_axis(void) {
  int status;
  int retcode = SUCCESS;
  
  /* test for each axes and set accdngly */
  /* X_TOP */
  if (st->axes & X_TOP) {
    status = init_xaxis(&st->x_top, d_xtop_y);
    if (status == FAILURE)
      goto axisfail;
  }
  /* X_DOWN */
  if (st->axes & X_DOWN) {
    status = init_xaxis(&st->x_down, d_xdown_y);
    if (status == FAILURE)
      goto axisfail;
  }
  /* Y_LEFT */
  if (st->axes & Y_LEFT) {
    status = init_yaxis(&st->y_left_arr, d_yleft_cnt);
    if (status == FAILURE)
      goto axisfail;
  }
  /* Y_RIGHT */
  if (st->axes & Y_RIGHT) {
    status = init_yaxis(&st->y_right_arr, d_yright_cnt);
    if (status == FAILURE)
      goto axisfail;
  }
  
 axisfail:
  return retcode;
}

/* intiallize the store
   Args: None
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
 */
int init_store(void) {
  int retcode  = SUCCESS;
  /* clear error */
  clr_error();

  /* make sure we don't end up in duplicate store creation, singleton? */
  if (st != NULL) {
    set_error(E_VM_WRONGVAL, "duplicate store creation");
    retcode = FAILURE;
    goto storefail;
  }

  /* create the store */
  st = (store *)malloc(sizeof(store));
  if (st == NULL) {
    set_error(E_VM_MALLOC, "(malloc) Store Init Failed");
    retcode = FAILURE;
    goto storefail;
  }
  /* set all to default values */
  st->axes = 0;
  st->x_top = NULL;
  st->x_down = NULL;
  st->y_left_arr = NULL;
  st->y_right_arr = NULL;
  
  /* set the axes count */
  set_label_axis(&st->axes);
  
  /* initialize each axis */
  retcode = init_axis();

 storefail:
  return retcode;
}

/* destroy y-axis */
static void destroy_yaxis(yaxis **y, int count) {
  int i;
  for(i = 0; i < count; i++) {  /* we need to clean the array pointed by the ptr */
    free(y[i]->val);
    free(y[i]);
  }
  free(y);
}

/* destroy x-axis */
static void destroy_xaxis(xaxis *x) {
  free(x->row.val);
  free(x->row.index_arr);
  free(x);
}

/* destroy the axes */
static void destroy_axis(void) {
  if (st->x_top != NULL) {
    destroy_xaxis(st->x_top);
  }

  if (st->x_down != NULL) {
    destroy_xaxis(st->x_down);
  }

  if (st->y_left_arr != NULL) {
    destroy_yaxis(st->y_left_arr, d_yleft_cnt);
  }

  if (st->y_right_arr != NULL) {
    destroy_yaxis(st->y_right_arr, d_yright_cnt);
  }
}

/* destory the store */
void destroy_store(void) {
  destroy_axis();
  free(st);
}


/* dump the store */
void dump_store(void) {
  int i;
  int cnt;
  fprintf(stderr, "---------------------------------- Store Dump ----------------------------------\n");
  fprintf(stderr, "Fields: [%d] X_TOP-to-Y: [%d] X_DOWN-to-Y: [%d]\n", opt_fields, d_xdown_y, d_xtop_y);
  fprintf(stderr, "X_DOWN: [%d] X_TOP: [%d] Y_LEFT: [%d] Y_RIGHT: [%d]\n", d_xdown_cnt, d_xtop_cnt, d_yleft_cnt, d_yright_cnt);
  /* header */
  cnt = d_xdown_cnt;
  while(cnt--) { fprintf(stderr, "%10s:xd",st->x_down->xinfo.name);}
  i = 0;
  cnt = d_yleft_cnt;
  while(cnt--) { fprintf(stderr, "%10s:yl",st->y_left_arr[i++]->yinfo.name); }
  cnt=d_xtop_cnt;
  while(cnt--) { fprintf(stderr, "%10s:yl",st->x_top->xinfo.name);}
  i = 0;
  cnt=d_yright_cnt;
  while(cnt--) { fprintf(stderr, "%10s:yr",st->y_right_arr[i++]->yinfo.name); }
  fprintf(stderr, "\n");        /* newline after header */
  return;
}

// gcc vmplot.h store.h store.c getopts.c -o store && ./store
/*
int main(void) {
  int status;
  status = init_store();
  if (status == SUCCESS)
    destroy_store();
  return 0;
}
*/
