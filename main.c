#include "main.h"

static ssize_t my_driver_write(struct file *file, const char *buf, size_t length, loff_t *ofs)
{
    ssize_t retval;
    printk(KERN_INFO "simple_buffer_write!\n");
    if (length > BUF_SIZE) {
        retval = -ENOMEM;
        return retval;
    }
    cur_len = retval = length;
    if (copy_from_user(memory_buffer, buf, retval)) {
        retval = -EFAULT;
        return retval;
    }
    printk(KERN_INFO "drv_buffer_write: [%zu bytes written]\n", retval);
	user_interface_handler(); // 유저 입력으로 LED 제어
    return retval;
}

static ssize_t my_driver_read(struct file *file, char *buf, size_t length, loff_t *ofs)
{
    ssize_t retval;
    printk(KERN_INFO "simple_buffer_read!\n");
    if (*ofs > 0) {
        return 0;
    }
    retval = cur_len;
	memory_buffer = (void *)led_flag[2];
    if (copy_to_user(buf, memory_buffer, retval)) {
        retval = -EFAULT;
        return retval;
    }
    printk(KERN_INFO "drv_buffer_read: [%zu bytes read]\n", retval);
    *ofs += retval;
    return retval;
}

static int my_driver_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_buffer_open!\n");
    memory_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
    if (!memory_buffer) {
        printk(KERN_ERR "Error: allocating memory for the buffer\n");
        return -ENOMEM;
    }
    memset(memory_buffer, 0, BUF_SIZE);
    return 0;
}

static int my_driver_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_buffer_release!\n");
    if (memory_buffer) {
        kfree(memory_buffer);
    }
    return 0;
}

static int my_driver_init(void)
{
    printk(KERN_INFO "my_driver_init!\n");
    if (register_chrdev(DEV_MAJOR_NUMBER, DRIVER_NAME, &my_driver_fops) < 0) {
        printk(KERN_ERR "driver_exam register failed!\n");
        return -1;
    }
    return 0;
}

static void my_driver_exit(void)
{
    printk(KERN_INFO "my_driver_exit!\n");
    unregister_chrdev(DEV_MAJOR_NUMBER, DRIVER_NAME);
}


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

    int ret = 0, i = 0;
	led_flag[0] = 1;
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

// static void user_module_led_mode_3(void) 
// {
// 	if (is_on_led_flag())
// 		return;
// 	led_flag[2] = 1;
// }

static void on_off_led_mode_3(short idx)
{
	int ret;

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

// user interface handler : 유저가 입력한 값을 여기서 처리
static void user_interface_handler(void)
{
	char mode = *((char*)memory_buffer);
	printk("mode : %c\n", mode);

	if (led_flag[2] == 0)
	{
		switch(mode){
			case '1':
				printk(KERN_INFO "sw1 interrupt ocurred!\n");
				module_led_mode_1();
				break;
			case '2':
				printk(KERN_INFO "sw2 interrupt ocurred!\n");
				module_led_mode_2();
				break;
			case '3':
				printk(KERN_INFO "sw3 interrupt ocurred!\n");
				module_led_mode_3();
				break;
			case '4':
				printk(KERN_INFO "sw4 interrupt ocurred!\n");
				module_led_mode_4();
				break;
		}
	}
	else
	{
		switch(mode){
			case '1':
				printk(KERN_INFO "sw1 interrupt ocurred!\n");
				on_off_led_mode_3(1);
				break;
			case '2':
				printk(KERN_INFO "sw2 interrupt ocurred!\n");
				on_off_led_mode_3(2);
				break;
			case '3':
				printk(KERN_INFO "sw3 interrupt ocurred!\n");
				on_off_led_mode_3(3);
				break;
			case '4':
				printk(KERN_INFO "sw4 interrupt ocurred!\n");
				module_led_mode_4();
				break;
		}
	}
}

irqreturn_t irq_handler(int irq, void *dev_id)
{
	printk(KERN_INFO "Debug %d\n", irq);

	if (led_flag[2] == 0) // 3번 모드가 켜져있지 않을 때
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

	my_driver_init(); // driver init
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

	my_driver_exit(); // driver exit
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
