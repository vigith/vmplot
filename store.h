/* common header for all the data transformations (input and output) and storage */

#include "vmplot.h"


/* tranformation functions */
/* input tranfsorm */
typedef void (*input_fn)(void **);
/* output tranfsorm */
typedef void (*output_fn)(void **);

/* struct holding all the transformation functions for each data point */
typedef struct {
  input_fn i_fn;
  output_fn o_fn;
  /* Future Use
  // .. aggregate func? for output
  */
} trf_fn;

/* data type for each data point */
typedef enum {
  LONG,                         /* long */
  FLOAT,                        /* float */
  TIME,                         /* time (epoch) */
} data_type;

/* types of axes for labelling.
   labels will be inferred from the max and min 
   values of each axis. (for y it will be min and max
   w.r.t all the data points, since we have multiple
   data points for y) */
typedef enum {
  X_DOWN  = (1 << 0),
  X_TOP   = (1 << 1),
  Y_LEFT  = (1 << 2),
  Y_RIGHT = (1 << 3),
} label_axis; 


/* basic data store info */
typedef struct {
  char name[48];           /* name of the field */
  data_type d_type;        /* data type */
  trf_fn *t_fn_ptr;        /* transformation function pointers  */
  int current_idx;         /* current index */
  void *max;               /* maximum value, depends on "data type" */
  void *min;               /* minimum value, depends on "data type" */
  int color[3];            /* RGB color */
  char plot;               /* plot char */
  label_axis l_axis;       /* label axis */
} info;

/* each datum (data point) */
typedef struct {
  void *value;                  /* the real data */
} datum;

/* x axis will have a data ptr to all the y data sets */
typedef struct {
  datum *val;     /* value of xaxis point */
  int *index_arr; /* index of each y data set, pointer to integer array (index is smaller, but ptr will be faster) */
} table;

/* x axis */
typedef struct {
  info xinfo;
  table row;                    /* all the data for x axis */
} xaxis;

/* y axis data sets */
typedef struct {
  info yinfo;
  datum *val;                   /* all the data for y axis */
} yaxis;

/* the store for the data points */
typedef struct {
  int axes; /* how many do we have for x and y (set by using label_axis) */
  xaxis *x_down;
  xaxis *x_top;
  yaxis **y_left_arr;               /* pointer to pointer of array */
  yaxis **y_right_arr;
} store;



/***********/
/* GLOBALS */
/***********/
extern store *st;
