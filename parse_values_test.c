#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include "parse_values.h"

/* test istime */
void test_istime(void **state) {
  int result;
  result = istime("1462983088", NULL);
  assert_int_equal(result, SUCCESS);
  result = istime("1462983088", "%s");
  assert_int_equal(result, SUCCESS);
  return;
}

/* test istime fail */
void test_istime_fail(void **state) {
  int result;
  result = istime("1462983088", "%Y-%m");
  assert_int_equal(result, FAILURE);
  result = istime("1462983088-0", NULL);
  assert_int_equal(result, FAILURE);
  return;
}

/* test the failure float conversions */
void test_convert_string_to_float_fail(void **state) {
  float num = 0;
  char *str[] = {
    "10.a",
    "a10",
  };
  int status = 0;
  int i = 0;
  size_t str_cnt = sizeof(str) / sizeof(char *);
  for(i=0; i<str_cnt; i++) {
    status = string_to_float(str[i], &num);
    assert_int_equal(status, FAILURE);
  }
  return;
}


/* test the failure long conversions */
void test_convert_string_to_long_fail(void **state) {
  long num = 0;
  char *str[] = {
    "10.0",
    "10a",
  };
  int status = 0;
  int i = 0;
  size_t str_cnt = sizeof(str) / sizeof(char *);
  for(i=0; i<str_cnt; i++) {
    status = string_to_long(str[i], &num);
    assert_int_equal(status, FAILURE);
  }
  return;
}

/* test string to float conversion */
void test_convert_string_to_float(void **state) {
  float num = 0;
  char *str[] = {
    "10",
    "10.001",
  };
  int status = 0;
  int i = 0;
  size_t str_cnt = sizeof(str) / sizeof(char *);
  for(i=0; i<str_cnt; i++) {
    status = string_to_float(str[i], &num);
    assert_int_equal(status, SUCCESS);
  }
  return;
}


void test_convert_string_to_long(void **state) {
  long num = 0;
  char *str[] = {
    "10"
  };
  int status = 0;
  int i = 0;
  size_t str_cnt = sizeof(str) / sizeof(char *);
  for(i=0; i<str_cnt; i++) {
    status = string_to_long(str[i], &num);
    assert_int_equal(status, SUCCESS);
  }
  return;
}

/* start the tests */
int main(int argc, char *argv[]) {
  const UnitTest tests[] = {
    unit_test(test_istime),
    unit_test(test_convert_string_to_float),
    unit_test(test_convert_string_to_long),
    /* failures */
    unit_test(test_convert_string_to_float_fail),
    unit_test(test_convert_string_to_long_fail),
    unit_test(test_istime_fail),
  };
  return run_tests(tests);
}


// gcc -I /usr/local/include/google/ -lcmockery vmplot.h parse_time.c parse_time.h parse_values.h parse_values.c parse_values_test.c -o parse_values && ./parse_values
