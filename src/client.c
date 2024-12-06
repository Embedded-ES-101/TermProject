#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "client.h"

#define BUF_SIZE 10
#define DRIVER_NAME "/dev/my_driver"

void led_enter(int dev)
{
	char buf[BUF_SIZE];
	int led[4] = {0, 0, 0, 0};
	int len;

	while (1) {
		write(0, "LED to enable: ", 15);
		if ((len = read(0, buf, BUF_SIZE)) < 0) {
			perror("read");
			exit(1);
		}

		if (buf[0] != '1' || buf[0] != '2' || buf[0] != '3' || buf[0] != '4') {
			write(0, "Invalid LED\n", 12);
			continue;
		}

		if (len != 2) {
			write(0, "Invalid LED\n", 12);
			continue;
		}

		write(dev, buf, 1);
		if (buf[0] == '4') 
			break;
	}
}

int main(void)
{
	int dev;

	// 드라이버 열기
	dev = open(DRIVER_NAME, O_RDWR);
	if (dev < 0)
	{
		write(0, "driver open failed!\n", 21);
		return -1;
	}

	// 명령어 실행
	while (1) {
		char *cmd = (char *)malloc(sizeof(char) * BUF_SIZE);
		int len;
		if (cmd == NULL) {
			perror("malloc");
			exit(1);
		}

		write(0, "Mode 1: 1\n", 10);
		write(0, "Mode 2: 2\n", 10);
		write(0, "Mode 3: 3\n", 10);
		write(0, "Mode 4: 4\n", 10);

		write(0, "Type a mode: ", 13);

		if ((len = read(0, cmd, BUF_SIZE)) < 0) {
			perror("read");
			exit(1);
		}
		
		if (len != 2) {
			printf("Invalid mode\n");
			continue;
		}

		switch (cmd[0]) {
		case '1':
			write(dev, cmd, 1);
			break;
		case '2':
			write(dev, cmd, 1);
			break;
		case '3':
			write(dev, cmd, 1);
			led_enter(dev);
			break;
		case '4':
			write(dev, cmd, 1);
			break;
		default:
			printf("Invalid mode\n");
			break;
		}

		free(cmd);
		write(0, "---------------------------\n", 30);
	}

	// write(dev, &buf, 1); // 커널로 데이터 전송
	// read(dev, &buf, 1); // 커널로부터 데이터 수신
	close(dev);
	return 0;
}