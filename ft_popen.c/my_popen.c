#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
	if (!file || !argv || (type != 'r' && type != 'w'))
		return -1;
	int	fd[2];
	pid_t pid;

	if (pipe(fd) < 0)
		return -1;
	pid = fork();
	if (pid < 0)
	{
		close(fd[1]);
		close(fd[0]);
		return -1;
	}
	if (pid == 0)
	{
		if (type == 'r')
		{
			close(fd[0]);
			if (dup2(fd[1], STDOUT_FILENO) < 0)
			{
				close(fd[0]);
				close(fd[1]);
				return -1;
			}
		}
		else
		{
			close(fd[1]);
			if (dup2(fd[0], STDIN_FILENO) < 0)
			{
				close(fd[0]);
				close(fd[1]);
				return -1;
			}
		}
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(-1);
	}
	else
	{
		if (type == 'r')
		{
			close(fd[1]);
			return (fd[0]);
		}
		else
		{
			close(fd[0]);
			return (fd[1]);
		}
	}
}