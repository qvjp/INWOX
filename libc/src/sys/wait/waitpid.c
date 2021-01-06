#include <sys/syscall.h>
#include <sys/wait.h>

DEFINE_SYSCALL_GLOBAL(SYSCALL_WAITPID, pid_t, waitpid, (pid_t, int *, int));
