#include <proto/exec.h>
#include <unistd.h>

pid_t getpid(void) {
  return (pid_t)FindTask(NULL);
}
