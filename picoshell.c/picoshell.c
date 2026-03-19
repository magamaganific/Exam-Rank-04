#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int	picoshell(char **cmds[])
{
	int		i = 0;
	int		fd[2];
	int		in_fd = 0;
	int		res = 0;
	int		status;
	pid_t	pid;

	while (cmds[i])
	{
		if (cmds[i + 1]) // if  there are more commands
		{
			if (pipe(fd) == -1) // do a pipe
				return (1);
		}
		else // if there are no commands therefore no pipe
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid < 0) // fail safeguard
		{
			if (fd[0] != -1)
				close(fd[0]); // close the read fd
			if (fd[1] != -1)
				close(fd[1]); //close write fd
			if (in_fd != 0)
				close(in_fd);
			return (1);
		}
		if (pid == 0) // child detected >:)
		{
			if (in_fd != 0) // checking for previous pipe
			{
				if (dup2(in_fd, 0) == -1) // establish the in fd
					exit(1);
				close(in_fd);
			}
			if (fd[1] != -1) // connecting stdout
			{
				if (dup2(fd[1], 1) == -1) // establish write fd as stdout
					exit(1);
				close(fd[1]);
				close(fd[0]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else
		{
			if (in_fd != 0)
				close(in_fd);
			if (fd[1] != -1)
				close(fd[1]); // el padre cierra el write.
			in_fd = fd[0];
			i++;
		}
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			res = 1;
		else if (!WIFEXITED(status))
			res = 1;
	}
	return (res);
}