#include "store.h"

store *st = NULL;               /* start with an NULL pointer, easy to free */

/*  TODO: write it properly
 */
void set_label_axis(int *axes) {
  *axes |= X_DOWN;
  *axes |= Y_LEFT;
  return;
}

/* intialize the Y-Axis
   The fuction will free all the allocated memory that happened
   in this fuction if there is an error.
   Args: None
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
   Args: None
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
static int init_store(void) {
  int retcode  = SUCCESS;
  /* clear error */
  clr_error();

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
  init_axis();

  printf("%d\n",sizeof(*st));

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
static void destroy_store(void) {
  destroy_axis();
  free(st);
}

int main(void) {
  int status;
  status = init_store();
  if (status == SUCCESS)
    destroy_store();
  return 0;
}
