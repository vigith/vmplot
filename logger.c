#include "logger.h"

/* creates a logfile and returns the file descriptor
   Args: logfile name
   Returns:
    - fd
   ERROR: will abort() on error
*/
int open_logfile(char *logfile) {
  int _logfd;

  _logfd  = open(logfile,  O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IRGRP|S_IWRITE|S_IWGRP);
  if (_logfd == -1) {
    _LOGGER("CRIT# (exiting) Can't open file [%s] for writing [%s]", logfile, strerror(errno));
    abort();
  }

  return _logfd;
}
