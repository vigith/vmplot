#include "draw.h"

/* externs */
int width;
int height;
caca_canvas_t *cv;
caca_display_t *dp;
caca_event_t ev;
graph *gr;
store *st;

/* intiallizes the graph axis
   - their start and end coords
   - move to proper origin
   Args: None
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
 */
int init_graph(void) {
  int status = SUCCESS;
  char _errstr[128];

  /* initiallize the graph */
  gr = (graph *)malloc(sizeof(graph));
  if (gr == NULL) {
    set_error(E_VM_MALLOC, "cannot initiallize graph");
    return FAILURE;
  }
  
  /* deciding the right origin, (0, 0) is on the top left corner, we need to bring that down
   * Origin: origin is a subjective word, in graph paper, origin is (0,0). for our case origin is
             specific to each axis. */
  /* X Down */
  /* if Y-Left is missing, we have more space on the left */
  if (! st->axes & Y_LEFT) gr->xd.start.x = 0 + VM_PADDING;
  else gr->xd.start.x = 0 + VM_PADDING + opt_label_vert;
  /* y coord won't change, if we have x-down we need some space for labelling */
  gr->xd.start.y = height - VM_PADDING - opt_label_horz;
  /* if Y-Right is missing, we have more space on the right */
  if (! st->axes & Y_RIGHT) gr->xd.end.x = width - VM_PADDING;
  else gr->xd.end.x = width - VM_PADDING - opt_label_vert;
  /* y remains on the same level as of the start point */
  gr->xd.end.y = gr->xd.start.y;
  
  /* X Top */
  /* if Y-Left is missing, we have more space on the left */
  if (! st->axes & Y_LEFT) gr->xt.start.x = 0 + VM_PADDING;
  else gr->xt.start.x = 0 + VM_PADDING + opt_label_vert;
  gr->xt.start.y = 0 + VM_PADDING + opt_label_horz;
  /* if Y-Right is missing, we have more space on the right */
  if (! st->axes & Y_RIGHT) gr->xt.end.x = width - VM_PADDING;
  else gr->xt.end.x = width - VM_PADDING - opt_label_vert;
  /* y remains on the same level as of the start point */
  gr->xt.end.y = gr->xt.start.y;

  /* Y Left */
  gr->yl.start.x = 0 + VM_PADDING + opt_label_vert;
  /* if X-Down is missing, we have more space on the bottom */
  if (! st->axes & X_DOWN) gr->yl.start.y = height - VM_PADDING;
  else gr->yl.start.y = height - VM_PADDING - opt_label_horz;
  /* x remains at same level */
  gr->yl.end.x = gr->yl.start.x;
  /* if X-Top is missing, we have more space on the top */
  if (! st->axes & X_TOP) gr->yl.end.y = 0 + VM_PADDING;
  else gr->yl.end.y = 0 + VM_PADDING + opt_label_horz;

  /* Y Right */
  gr->yr.start.x = width - VM_PADDING - opt_label_vert;
  /* if X-Down is missing, we have more space on the bottom */
  if (! st->axes & X_DOWN) gr->yr.start.y = height - VM_PADDING;
  else gr->yr.start.y = height - VM_PADDING - opt_label_horz;
  /* x remains at same level */
  gr->yr.end.x = gr->yr.start.x;
  /* if X-Top is missing, we have more space on the top */
  if (! st->axes & X_TOP) gr->yr.end.y = 0 + VM_PADDING;
  else gr->yr.end.y = 0 + VM_PADDING + opt_label_horz;

  _LOGGER_F(logfd, "xd:(%d, %d - %d, %d) xt:(%d, %d - %d, %d)", gr->xd.start.x, gr->xd.start.y, gr->xd.end.x, gr->xd.end.y, gr->xt.start.x, gr->xt.start.y, gr->xt.end.x, gr->xt.end.y);
  _LOGGER_F(logfd, "yl:(%d, %d - %d, %d) yr:(%d, %d - %d, %d)", gr->yl.start.x, gr->yl.start.y, gr->yl.end.x, gr->yl.end.y, gr->yr.start.x, gr->yr.start.y, gr->yr.end.x, gr->yr.end.y);

  
  return status;
}

void destory_graph(void) {
  free(gr);
}

/* intiallizes the canvas and display
   Args: None
   Returns:
    - SUCCESS
    - FAILURE
   ERROR: will set vmplot_errno and vmplot_errstr on error
 */
int init_canvas(void) {
  char _errstr[128];
  clr_error();
  /* create the display */
  dp = caca_create_display(NULL);
  if(!dp) {
    sprintf(_errstr, "caca_create_display error:%s", strerror(errno));
    set_error(E_VM_EXTFNCTN, _errstr);
    return FAILURE;
  }

  /* get the canvas from display */
  cv = caca_get_canvas(dp);     /* this func never fails */

  /* set width and height */
  height = caca_get_canvas_height(cv);
  width = caca_get_canvas_width(cv);

  return SUCCESS;
}

/* frees the canvas */
void destroy_canvas(void) {
  if (dp)
    caca_free_display(dp);
  return;
}

int main(void) {
  caca_canvas_t *cv; caca_display_t *dp; caca_event_t ev;

  if (init_store() == FAILURE)
    goto end1;

  // TODO: only for debugging
  logfd = open_logfile(d_logloc);  

  // init canvas
  if (init_canvas() == FAILURE)
    goto end2;

  if (init_graph() == FAILURE)
    goto end3;


 end3:
  destory_graph();
 end2:
  destroy_canvas();
 end1:
  destroy_store();

  return 0;
}

//  gcc -g -lcaca vmplot.h getopts.c store.c parse_values.h parse_values.c parse_time.c parse_time.h logger.h logger.c draw.h draw.c  -o draw && ./draw
