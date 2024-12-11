# Makefile for my_driver module with header file

# 모듈 이름
obj-m := my_driver.o

SRCS = main.o\

# 모듈에 포함될 소스 파일
my_driver-objs := $(SRCS)

# 커널 빌드 디렉토리 경로
KDIR := /lib/modules/$(shell uname -r)/build

# 현재 디렉토리
PWD := $(shell pwd)

INC_DIR = $(PWD)/inc


# 기본 타겟
all: 
	make -C $(KDIR) M=$(PWD) modules
	
# 클린 타겟
clean fclean:
	make -C $(KDIR) M=$(PWD) clean

# 설치 타겟 (선택 사항)
install:
	sudo insmod my_driver.ko
	sudo mknod /dev/my_driver c 220 0
	sudo chmod 666 /dev/my_driver

# 제거 타겟 (선택 사항)
uninstall:
	sudo rmmod my_driver
	sudo rm /dev/my_driver

