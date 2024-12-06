#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define HIGH 1
#define LOW 0

#define MODE3_FLAG1 (1 << 0)  // 0000 0001
#define MODE3_FLAG2 (1 << 1)  // 0000 0010 
#define MODE3_FLAG3 (1 << 2)  // 0000 0100
#define MODE3_FLAG4 (1 << 3)  // 0000 1000

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

// util
int is_on_led_flag(void)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		if (led_flag[i])
			return 1;
	}
	return 0;
}

#pragma region module_led_mode_1
static void timer_cb_mode1(struct timer_list *timer)
{
	int ret,i;
	printk(KERN_INFO "timer callback function ! \n");
	if(mode1_flag == 0){
		for(i=0;i<4;i++){
			ret = gpio_direction_output(led[i], HIGH);
		}
		mode1_flag=1;
	}
	else{
		for(i=0;i<4;i++){
			ret = gpio_direction_output(led[i], LOW);
		}
		mode1_flag=0;
	}
	timer->expires = jiffies + HZ * 2;
	add_timer(timer);
}

static int timer_module_init_mode1(void)
{
	int ret,i;
	printk(KERN_INFO "led_module_init!\n");
	for(i=0;i<4;i++){
		ret = gpio_request(led[i], "LED");
		if(ret<0)
			printk(KERN_INFO "led_module gpio request failed!\n");
	}
	timer_setup(&timer, timer_cb_mode1, 0);
	timer.expires = jiffies + HZ * 2;
	add_timer(&timer);
	return 0;
}

static void module_led_mode_1(void)
{
	if (is_on_led_flag())
		return;

	led_flag[0] = 1;
    int ret, i;
    if(mode1_flag == 0){
		for(i=0;i<4;i++){
			ret = gpio_direction_output(led[i], HIGH);
		}
		mode1_flag=1;
	}

	timer_module_init_mode1();
}
#pragma endregion

#pragma region module_led_mode_2
static void timer_cb_mode2(struct timer_list *timer)
{
	int ret;

    ret = gpio_direction_output(led[led_number], HIGH);
    ret = gpio_direction_output(led[(led_number + 4 - 1) % 4], LOW);

    led_number = (led_number + 1) % 4;
	timer->expires = jiffies + HZ * 2;
	add_timer(timer);
}

static int timer_module_init_mode2(void)
{
	int ret,i;
	printk(KERN_INFO "led_module_init!\n");
	for(i=0;i<4;i++){
		ret = gpio_request(led[i], "LED");
		if(ret<0)
			printk(KERN_INFO "led_module gpio request failed!\n");
	}
	timer_setup(&timer, timer_cb_mode2, 0);
	timer.expires = jiffies + HZ * 2;
	add_timer(&timer);
	return 0;
}

static void module_led_mode_2(void)
{
	if (is_on_led_flag())
		return;
	led_flag[1] = 1;

	timer_module_init_mode2();
}
#pragma endregion

#pragma region module_led_mode_3

static void on_off_led_mode_3(short idx)
{
	switch(idx)
	{
		case 1:
			mode3_flags ^= MODE3_FLAG1;
			if (mode3_flags & MODE3_FLAG1) {
				ret = gpio_direction_output(led[0], HIGH);
			}
			else {
				ret = gpio_direction_output(led[0], LOW);
			}
			break;
		case 2:
			mode3_flags ^= MODE3_FLAG2;
			if (mode3_flags & MODE3_FLAG2) {
				ret = gpio_direction_output(led[1], HIGH);
			}
			else {
				ret = gpio_direction_output(led[1], LOW);
			}
			break;
		case 3:
			mode3_flags ^= MODE3_FLAG3;
			if (mode3_flags & MODE3_FLAG3) {
				ret = gpio_direction_output(led[2], HIGH);
			}
			else {
				ret = gpio_direction_output(led[2], LOW);
			}
			break;
		default:
			return;
	}
}

static void module_led_mode_3(void)
{
	if (is_on_led_flag())
		return;
	led_flag[2] = 1;
}

#pragma endregion

#pragma region module_led_mode_4_reset

static void module_led_mode_4(void)
{
	int ret, i;
	printk(KERN_INFO "mode4 stop!\n");

	for(i=0;i<4;i++){
		ret = gpio_direction_output(led[i], LOW);
    }

	if (led_flag[0]) // 1번 led
	{
		mode1_flag = 0;
		del_timer(&timer);
		led_flag[0] = 0;
	}

    else if (led_flag[1]) // 2번 led
    {
        led_number = 0;
        del_timer(&timer);
		led_flag[1] = 0;
    }

	else if (led_flag[2]) // 3번 led
    {
        mode3_flags = 0;
		led_flag[2] = 0;
    }
}

#pragma endregion


irqreturn_t irq_handler(int irq, void *dev_id)
{
	printk(KERN_INFO "Debug %d\n", irq);

	if (led_flag[2] == 0)
	{
		switch(irq){
			case 60:
				printk(KERN_INFO "sw1 interrupt ocurred!\n");
				module_led_mode_1();
				break;
			case 61:
				printk(KERN_INFO "sw2 interrupt ocurred!\n");
				module_led_mode_2();
				break;
			case 62:
				printk(KERN_INFO "sw3 interrupt ocurred!\n");
				module_led_mode_3();
				break;
			case 63:
				printk(KERN_INFO "sw4 interrupt ocurred!\n");
				module_led_mode_4();
				break;
		}
	}
	
	else
	{
		int ret;

		switch(irq){
			case 60:
				printk(KERN_INFO "sw1 interrupt ocurred!\n");
				on_off_led_mode_3(1);
				break;
			case 61:
				printk(KERN_INFO "sw2 interrupt ocurred!\n");
				on_off_led_mode_3(2);
				break;
			case 62:
				printk(KERN_INFO "sw3 interrupt ocurred!\n");
				on_off_led_mode_3(3);
				break;
			case 63:
				printk(KERN_INFO "sw4 interrupt ocurred!\n");
				module_led_mode_4();
				break;
		}
	}
	return 0;
}

static int switch_interrupt_init(void){
	int res,i;
	printk(KERN_INFO "switch_interrupt_init!\n");
	for(i=0;i<4;i++){
		res = gpio_request(sw[i],"sw");
		res = request_irq(gpio_to_irq(sw[i]),(irq_handler_t)irq_handler,IRQF_TRIGGER_RISING,"IRQ",(void *)(irq_handler));
		if(res<0)
			printk(KERN_INFO "request_irq failed!\n");
	}
	return 0;
}

static void switch_interrupt_exit(void){
	int i;
	printk(KERN_INFO "switch_interrupt_exit!\n");
	del_timer(&timer);
	for(i=0;i<4;i++){
		free_irq(gpio_to_irq(sw[i]),(void *)(irq_handler));
		gpio_free(sw[i]);
		gpio_free(led[i]);
	}
}



module_init(switch_interrupt_init);
module_exit(switch_interrupt_exit);
MODULE_LICENSE("GPL");
