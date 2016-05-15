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

/* each datum (data point) */
typedef struct {
  void *value;                  /* the real data */
} datum;

/* basic data store info */
typedef struct {
  data_type d_type;        /* data type */
  trf_fn t_fn_ptr;         /* transformation function pointers  */
  int current_idx;         /* current index */
  void *max;               /* maximum value, depends on "data type" */
  void *min;               /* minimum value, depends on "data type" */
  int color[3];            /* RGB color */
  char plot;               /* plot char */
} info;

/* x axis will have a data ptr to all the y data sets */
typedef struct {
  datum value;                  /* value of xaxis point */
  int index[];                  /* index of each y data set */
} table;

/* x axis */
typedef struct {
  info *xinfo;
  table row[];                  /* all the data for x axis */
} xaxis;

/* y axis data sets */
typedef struct {
  info *yinfo;
  datum value[];                /* all the data for y axis */
} yaxis;

/* the store for the data points */
typedef struct {
  xaxis *x;
  yaxis *y[];                  /* multiple lines can be drawn against the same xaxis */
} store;
