#include <unistd.h>     
#include <sys/wait.h>   
#include <signal.h>     
#include <stdbool.h>    
#include <stdio.h>      
#include <stdlib.h>     
#include <errno.h>      
#include <string.h>   

static void do_nothing(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	if (!f)
		return(-1);
	int		status;
	pid_t	pid = fork();

	if(pid < 0)
		return(-1);
	if (pid == 0)
	{
		alarm(timeout);
		f();
		_exit(0);
	}
	struct sigaction sa = {0};
	sa.sa_handler = do_nothing;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);
	alarm(timeout);
	pid_t r = waitpid(pid, &status, 0);
	if (r < 0)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
	if (WIFEXITED(status))
	{
		int code = WEXITSTATUS(status);
		if (code == 0)
		{
			if (verbose)
				printf("Nice function!\n");
			return (1);
		}
		if (verbose)
			printf("Bad function: exited with code %d\n", code);
		return (0);
	}
	if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		if (sig == (SIGALRM))
		{
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
		}
		else
		{
			if (verbose)
				
		}
	}
}