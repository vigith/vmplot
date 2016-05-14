#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include "parse_time.h"

/* tests the longest match */
void get_the_longest_time_format(void **state) {
  int result = -1;
  result = get_format_for_time("2016-05-11 09:11:28 AM");
  assert_int_equal(result, 0);
  return;
}


/* tests the last match */
void get_the_last_time_format(void **state) {
  int result = -1;
  result = get_format_for_time("1462983088");
  assert_int_equal(result, time_format_hint_cnt - 1);
  return;
}


/* tests time convertion to epoch */
void test_convert_time_to_epoch(void **state) {
  time_t epoch = -1;
  int index;
  char *time_str = "2016-05-11 09:11:28 AM";
  time_t expected_epoch = 1462983088;
  index = get_format_for_time(time_str);
  epoch = convert_time_to_epoch(time_str, index, NULL);
  assert_int_equal((int)epoch, (int)expected_epoch);
  return;
}

/* tests time convertion to epoch */
void test_convert_epoch_to_time(void **state) {
  time_t epoch = -1;
  int index;
  char *time_str = "2016-05-11 09:11:28 AM";
  char buf[255];
  
  index = get_format_for_time(time_str);
  epoch = convert_time_to_epoch(time_str, index, NULL);
  convert_epoch_to_time(epoch, index, NULL, buf, sizeof(buf));
  assert_string_equal(time_str, buf);
  assert_string_not_equal(buf, ""); /* negative test (can't be true :-) */
  return;
}

/* test all the time formats */
void test_all_time_format_conversions(void **state) {
  char *time_strings[] = {
    "2016-05-11 09:11:28 AM",
    "2016-05-11 09:11:28",
    "2016-05-11",
    "2016 05 11",
    "09:11:28 AM",
    "09:11:28",
    "1462983088",
  };
  time_t epoch = -1;
  int index;
  char *time_str = NULL;
  char buf[255];
  size_t time_strings_cnt = sizeof(time_strings) / sizeof(char *);
  int i = 0;

  /* test of all the given time strings */
  for (i = 0; i< time_strings_cnt; i++) {
    time_str = time_strings[i];
    index = get_format_for_time(time_str);
    epoch = convert_time_to_epoch(time_str, index, NULL);
    convert_epoch_to_time(epoch, index, NULL, buf, sizeof(buf));
    assert_string_equal(time_str, buf);    
  }


  /* do a reverse conversion for all the given time strings */
  for (i = time_strings_cnt - 1; i > 0; i--) {
    time_str = time_strings[i];
    index = get_format_for_time(time_str);
    epoch = convert_time_to_epoch(time_str, index, NULL);
    convert_epoch_to_time(epoch, index, NULL, buf, sizeof(buf));
    assert_string_equal(time_str, buf);    
  }
}

/* start the tests */
int main(int argc, char *argv[]) {
  const UnitTest tests[] = {
    unit_test(get_the_longest_time_format),
    unit_test(get_the_last_time_format),
    unit_test(test_convert_time_to_epoch),
    unit_test(test_convert_epoch_to_time),
    unit_test(test_all_time_format_conversions),
  };
  return run_tests(tests);
}


// gcc -I /usr/local/include/google/ -lcmockery vmplot.h parse_time.h parse_time.c parse_time_test.c -o parse_time && ./parse_time  
