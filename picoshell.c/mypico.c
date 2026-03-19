#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int    picoshell(char **cmds[])
{
	int i = 0;
	int status;
	int ret = 0;
	int fd[2];
	int in_fd = 0;
	pid_t pid;

	while(cmds[i])
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) < 0)
				return(1);
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if(pid < 0)
		{
			if (fd[0] != -1)
				close(fd[0]);
			if (fd[1] != -1)
				close(fd[1]);
			if (in_fd != 0)
				close(in_fd);
			return(1);
		}
		if(pid == 0)
		{
			if(in_fd != 0)
			{
				if(dup2(in_fd, 0) < 0)
					exit(1);
				close(in_fd);
			}
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) < 0)
					exit(1);
				close(fd[0]);
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else
		{
			if (fd[1] != -1)
				close(fd[1]);
			if(in_fd != 0)
				close(in_fd);
			in_fd = fd[0];
			i++;
		}
	}
	while(wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			ret = 1;
		else if (!WIFEXITED(status))
			ret = 1;
	}
	return(ret);
}
