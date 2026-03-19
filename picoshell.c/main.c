#include <stdio.h>
#include <unistd.h>

int	picoshell(char **cmds[]);

int main(void)
{
    write(1, "Test picoshell_short\n", 21);
    // char *cmd1[] = {"/bin/ls", "level-1", NULL};
    char *cmd1[] = {"/bin/ls", NULL};
    char *cmd2[] = {"/usr/bin/grep", "picoshell", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};

    int result = picoshell(cmds);
    printf("picoshell returned %d\n", result);

    return 0;
}