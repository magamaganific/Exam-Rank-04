#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

void do_nothing(int sig)
{
	(void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	if (!f)
		return(-1);
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		close(fd[1]);
		close(fd[0]);
		return(-1);
	}
	if (pid == 0)
	{
		alarm(timeout)
		f();
		_exit(0);
	}
	struct sigaction sa;
	sa.sa_handler = do_nothing;
	sigemptyset(&sa.sa_masks);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);
	alarm(timeout);
}