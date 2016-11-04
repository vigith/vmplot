#include "vmplot.h"
#include <unistd.h>


#define LOGSTDERR 2

/* 
 *  rudimentary logger 
 */
// log to a file
#define _LOGGER_F(fd, fmt, ...)                                         \
  do {                                                                  \
    if (fcntl(fd, F_GETFD) == -1 || errno == EBADF)                     \
      dprintf(LOGSTDERR, "%s:%d:%s()# !NOT A VALID LOG FD! " fmt "\n", __FILE__, __LINE__, __func__, __VA_ARGS__); \
    else                                                                \
      dprintf(fd, "%s:%d:%s()# " fmt "\n", __FILE__, __LINE__, __func__, __VA_ARGS__); \
  } while (0)

// log to fd
#define _LOGGER(fmt, ...) \
  do { dprintf(LOGSTDERR, "%s:%d:%s()# " fmt "\n", __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)

/***********/
/* Globals */
/***********/
int logfd;

