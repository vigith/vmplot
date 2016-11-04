#include <caca.h>
#include "vmplot.h"
#include "logger.h"
#include "store.h"

/* a point in the graph (is described by x and y coordinate) */
typedef struct {
  int x;
  int y;
} point;

/* each axis in the graph */
typedef struct {
  point start;                  /* origin of the axis */
  point end;                    /* end of the axis */
  long max;                     /* upper bound, though data type is different, we have to map it to "long" */
  long min;                     /* lower bound */
} graph_axis;

/* a graph is known by its axes */
typedef struct {
  graph_axis xd;                /* x down */
  graph_axis xt;                /* x top */
  graph_axis yl;                /* y left */
  graph_axis yr;                /* y right */
} graph;

/* dimensions of the screen */
extern int width;
extern int height;

/* libcaca stuff */
extern caca_canvas_t *cv;
extern caca_display_t *dp;
extern caca_event_t ev;

extern graph *gr;
