#include "vmplot.h"
#include "store.h"

/* long int array which contains which axis each field goes to and which index to keep
   for each field (ie, "D;L:D,L:D" means first field is X_DOWN and should be kepth at 
   index 0, second is Y_LEFT and kept at index 0, 3rd is Y_LEFT and kept at index 1.)
   each field in data2axis array is of 3 main parts
    - last 8 bits (LSB) have the axis info
    - next 16 bits have the index to where in array y_left_arr or y_right_arr (this
      means we can not have more than 16 plots for left or right y axis)
    - last 8 bits is to show whether data is plotted again X_DOWN or X_TOP (useful
      only for Y_LEFT and Y_RIGHT)
   XXX: ffs is used to find the right most set bit.
*/
extern long *data2axis;
