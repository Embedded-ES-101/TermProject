#ifndef MAIN_H
# define MAIN_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h> // copy_from_user, copy_to_user 함수 정의
#include <linux/slab.h>    // kmalloc, kfree 함수 정의

#define HIGH 1
#define LOW 0

#define MODE3_FLAG1 (1 << 0)  // 0000 0001
#define MODE3_FLAG2 (1 << 1)  // 0000 0010 
#define MODE3_FLAG3 (1 << 2)  // 0000 0100
#define MODE3_FLAG4 (1 << 3)  // 0000 1000

#define DEV_MAJOR_NUMBER 220
#define DRIVER_NAME "my_driver"
#define BUF_SIZE 10

int sw[4] = {4,17,27,22};
int led[4] = {23, 24, 25, 1};

int mode1_flag = 0; // mode1 led flag
int led_number; // mode2 led flag
int mode3_flags; // mode3 flags
//int mode2_flag[4];
int led_flag[4]; // mode flag 어떤 모드가 켜져있나?

static struct timer_list timer;

irqreturn_t irq_handler(int irq, void *dev_id);
static int is_on_led_flag(void);

// mode1 func
static void timer_cb_mode1(struct timer_list *timer);
static int timer_module_init_mode1(void);
static void module_led_mode_1(void);

// mode2 func
static void timer_cb_mode2(struct timer_list *timer);
static int timer_module_init_mode2(void);
static void module_led_mode_2(void);

// mode3 func
static void on_off_led_mode_3(short idx);
static void module_led_mode_3(void);

// mode4 func (reset mode)
static void module_led_mode_4(void);

// driver func
static int my_driver_open(struct inode *inode, struct file *file);
static int my_driver_release(struct inode *inode, struct file *file);
static ssize_t my_driver_write(struct file *file, const char *buf, size_t length, loff_t *ofs);
static ssize_t my_driver_read(struct file *file, char *buf, size_t length, loff_t *ofs);

static void user_interface_handler(void);
static int my_driver_init(void);
static void my_driver_exit(void);

void *memory_buffer;
ssize_t cur_len;


static struct file_operations my_driver_fops = {
    .owner = THIS_MODULE,
    .open = my_driver_open,
    .release = my_driver_release,
    .write = my_driver_write,
    .read = my_driver_read,
};

#endif