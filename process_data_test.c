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

void create_test_store(void **state) {
  int i;
  data2axis = (long *)malloc(sizeof(long) * 10); /* don't test for more than 10 elements, 10 should be replaced by opt_fields */
  bzero(data2axis, sizeof(long) * 10);
  *state = (long *)data2axis;

  // TODO: enable here
  //  init_store();
  
  return;
}


void destroy_test_store(void **state) {
  // TODO: enable here
  //  destroy_store();
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
  return;
}

/* success */
void test_set_fnargs_1(void **state) {
  int status = FAILURE;
  int i;
  const char *header[] = { "foo", "bar", "moo" };
  const char *input[] = { "2016-05-11 09:11:28 AM", "10", "11.25" };
  char *in_hint_args[] = { NULL, NULL, NULL };
  char *out_hint_args[] = { NULL, NULL, NULL };
  opt_fields = 3;
  opt_data2axis = "D;L:D;L:D";
  status = set_data2axis();
    init_store();
  
  status = set_store_axis_fnargs(in_hint_args, out_hint_args, input);
  assert_int_equal(status, SUCCESS);
  assert_string_equal(in_hint_args[0], "%Y-%m-%d %H:%M:%S %p");
  /* test xinfo details */
  assert_int_equal(st->x_down->xinfo.d_type, TIME);
  assert_int_equal(st->x_down->xinfo.t_fns.i_fn == store_str_as_time, 1);
  /* test yinfo details */
  assert_int_equal(st->y_left_arr[0]->yinfo.d_type, LONG); /* index 0 */
  assert_int_equal(st->y_left_arr[0]->yinfo.t_fns.i_fn == store_str_as_long, 1);
  assert_int_equal(st->y_left_arr[1]->yinfo.d_type, FLOAT); /* index 1 */
  assert_int_equal(st->y_left_arr[1]->yinfo.t_fns.i_fn == store_str_as_float, 1);
  /* cleanup in_hint_args */
  for (i=0; i<opt_fields; i++) {
    free(in_hint_args[i]);
    in_hint_args[i] = NULL;
  }

  // override wrong one via command line (failure)
  d_in_field_hints[0] = (char *)malloc(sizeof(char) * 50);
  strcpy(d_in_field_hints[0], "%Y-%m-%d %H:%M:%S"); // TIME# is missing, so we will fail
  status = set_store_axis_fnargs(in_hint_args, out_hint_args, input);
  assert_int_equal(status, FAILURE);
  /* cleanup derived d_in_field_hints */
  free(d_in_field_hints[0]);
  d_in_field_hints[0] = NULL;
  /* cleanup in_hint_args */
  for (i=0; i<opt_fields; i++) {    
    free(in_hint_args[i]);
    in_hint_args[i] = NULL;
  }
  
  // override via command line with format
  d_in_field_hints[0] = (char *)malloc(sizeof(char) * 50);
  strcpy(d_in_field_hints[0], "TIME#%Y-%m-%d %H:%M:%S"); // TIME# is missing, still it is okay we will guess it
  status = set_store_axis_fnargs(in_hint_args, out_hint_args, input);
  assert_int_equal(status, SUCCESS);
  assert_string_equal(in_hint_args[0], "%Y-%m-%d %H:%M:%S"); // command line hint matches the value set in set_store_axis_fnargs
  /* cleanup derived d_in_field_hints */
  free(d_in_field_hints[0]);
  d_in_field_hints[0] = NULL;
  /* cleanup in_hint_args */
  for (i=0; i<opt_fields; i++) {
    free(in_hint_args[i]);
    in_hint_args[i] = NULL;
  }
  
  /* cleanup */
  destroy_store();

  return;
}


/* failure */
/* TODO: write one, separate out store */
/* start the tests */
int main(int argc, char *argv[]) {
  const UnitTest tests[] = {
    /* data2axis */
    unit_test_setup_teardown(test_set_data2axis_1, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_2, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_3, create_data2axis_arr, destroy_data2axis_arr),
    unit_test_setup_teardown(test_set_data2axis_4, create_data2axis_arr, destroy_data2axis_arr),
    /* set_store_axis_fnargs */
    unit_test_setup_teardown(test_set_fnargs_1, create_test_store, destroy_test_store),
  };
  return run_tests(tests);
}
