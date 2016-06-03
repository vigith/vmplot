#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include "process_data.h"

void create_data2axis_arr(void **state) {
  int i;
  data2axis = (long *)malloc(sizeof(long) * 10); /* don't test for more than 10 elements, 10 should be replaced by opt_fields */
  bzero(data2axis, sizeof(long) * 10);
  *state = (long *)data2axis;
  return;
}


void destroy_data2axis_arr(void **state) {
  free(*state);
}

/* success */
void test_set_data2axis_1(void **state) {
  int status;
  int i;
  opt_fields = 3;
  opt_data2axis = "D;L:D;L:D";
  status = set_data2axis();
  assert_int_equal(data2axis[0], 257);
  assert_int_equal(data2axis[1], 16777476);
  assert_int_equal(data2axis[2], 16777732);
  assert_int_equal(data2axis[3], 0);
  return;
}

/* failure using , */
void test_set_data2axis_2(void **state) {
  int status;
  int i;
  opt_fields = 3;
  opt_data2axis = "D,L:D,L:D";
  status = set_data2axis();
  assert_int_equal(status, FAILURE);
  return;
}

/* failure using missing : */
void test_set_data2axis_3(void **state) {
  int status;
  int i;
  opt_fields = 2;
  opt_data2axis = "D;L";
  status = set_data2axis();
  assert_int_equal(status, FAILURE);
  return;
}

/* success */
void test_set_data2axis_4(void **state) {
  int status;
  int i;
  opt_fields = 5;
  opt_data2axis = "D;L:D;T;L:T;L:D";
  status = set_data2axis();
  assert_int_equal(data2axis[0], 257);      /* 100000001 */
  assert_int_equal(data2axis[1], 16777476); /* 1000000000000000100000100 */
  assert_int_equal(data2axis[2], 258);      /* 100000010 */
  assert_int_equal(data2axis[3], 33554948); /* 10000000000000001000000100 */
  assert_int_equal(data2axis[4], 16778244); /* 1000000000000010000000100 */
  assert_int_equal(data2axis[5], 0);        /* 0 */
  assert_int_equal(status, SUCCESS);
  printf(vmplot_errstr);
  return;
}

/* start the tests */
int main(int argc, char *argv[]) {
  const UnitTest tests[] = {
    unit_test_setup_teardown(test_set_data2axis_1, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_2, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_3, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_4, create_data2axis_arr, destroy_data2axis_arr),
  };
  return run_tests(tests);
}
