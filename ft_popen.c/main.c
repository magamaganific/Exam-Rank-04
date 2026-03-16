#include <unistd.h>
#include <stdlib.h>

int    ft_popen(const char *file, char *const argv[], char type);

int main(void)
{
    int  fd;
    char c;

    fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
    if (fd < 0)
        return (1);
    while (read(fd, &c, 1) > 0)
        write(1, &c, 1);
    close(fd);
    return (0);
}