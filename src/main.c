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

int sw[4] = {4,17,27,22};
int led[4] = {23, 24, 25, 1};
int flag = 0;
int led_flag[4];
static struct timer_list timer;

static void module_led_mode_1(void);
static void module_led_mode_4(void);

static void timer_cb(struct timer_list *timer)
{
	int ret,i;
	printk(KERN_INFO "timer callback function ! \n");
	if(flag == 0){
		for(i=0;i<4;i++){
			ret = gpio_direction_output(led[i], HIGH);
		}
		flag=1;
	}
	else{
		for(i=0;i<4;i++){
			ret = gpio_direction_output(led[i], LOW);
		}
		flag=0;
	}
	timer->expires = jiffies + HZ * 2;
	add_timer(timer);
}

static int timer_module_init(void)
{
	int ret,i;
	printk(KERN_INFO "led_module_init!\n");
	for(i=0;i<4;i++){
		ret = gpio_request(led[i], "LED");
		if(ret<0)
			printk(KERN_INFO "led_module gpio request failed!\n");
	}
	timer_setup(&timer, timer_cb, 0);
	timer.expires = jiffies + HZ * 2;
	add_timer(&timer);
	return 0;
}


static void module_led_mode_1(void)
{
	if (led_flag[0])
		return;
	timer_module_init();
	led_flag[0] = 1;
}

irqreturn_t irq_handler(int irq, void *dev_id){
	printk(KERN_INFO "Debug %d\n", irq);
	switch(irq){
		case 60:
			printk(KERN_INFO "sw1 interrupt ocurred!\n");
			module_led_mode_1();
			break;
		case 61:
			printk(KERN_INFO "sw2 interrupt ocurred!\n");
			break;
		case 62:
			printk(KERN_INFO "sw3 interrupt ocurred!\n");
			break;
		case 63:
			printk(KERN_INFO "sw4 interrupt ocurred!\n");
			module_led_mode_4();
			break;
	}
	return 0;
}

static void module_led_mode_4(void)
{
	int ret, i;
	printk(KERN_INFO "mode4 stop!\n");

	if (led_flag[0]) // 1번 led
	{
        if (flag)
        {
            for(i=0;i<4;i++){
			    ret = gpio_direction_output(led[i], LOW);
            }
            flag = 0;
		}

		del_timer(&timer);
		led_flag[0] = 0;
	}
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
