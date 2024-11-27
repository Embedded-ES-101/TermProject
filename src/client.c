#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char cmd[10];
	int dev;
	char buf = 1;
	dev = open("/dev/my_driver", O_RDWR);
	if (dev < 0)
	{
		printf("driver open failed!\n");
		return -1;
	}

	// 명령어 실행
	while (1) {
		if (read(0, cmd, 10) < 0)
		{
			perror("read");
			exit(1);
		}
	}



	write(dev, &buf, 1);
	read(dev, &buf, 1);
	close(dev);
	return 0;
}