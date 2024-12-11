#ifndef CLIENT_H
# define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 10
#define DRIVER_NAME "/dev/my_driver"

void print_title(void);
void print_subtitle(void);
void exit_banner();

#endif