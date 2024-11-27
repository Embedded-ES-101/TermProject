#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#define DEV_MAJOR_NUMBER 220
#define DEV_NAME "my_driver"

#define BUF_SIZE 10;

void *my_driver_buf;
ssize_t cur_len;

static struct file_operations my_driver_fops = {
	.owner = THIS_MODULE,
	.open = my_driver_open,
	.release = my_driver_release,
	.write = my_driver_write,
	.read = my_driver_read,
};

static int my_driver_write(struct file *file, const char *buf, size_t length, loff_t *ofs)
{
	ssize_t retval;
	printk(KERN_INFO "simple_buffer_write!\n");
	if (length > BUF_SIZE)
	{
		retval = -ENOMEM;
		return retval;
	}
	cur_len = retval = length;
	if (copy_from_user(memory_buffer, buf, retval))
	{
		retval = -EFAULT;
		return retval;
	}
	printk(KERN_INFO "drv_buffer_write: [%dbyte -> %dbyte]\n", length, retval);

	return retval;
}

static ssize_t my_driver_read(struct file *file, char *buf, size_t length, loff_t *ofs)
{
	ssize_t retval;
	printk(KERN_INFO "simple_buffer_read!\n");
	if (*ofs > 0)
	{
		retval = 0;
		return retval;
	}
	retval = cur_len;
	if (copy_to_user(buf, memory_buffer, retval))
	{
		retval = -EFAULT;
		return retval;
	}
	printk(KERN_INFO "drv_buffer_read: [%dbyte -> %dbyte]\n", length, retval);
	*ofs += retval;

	return retval;
}

static int my_driver_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "simple_buffer_open!\n");
	memory_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
	if (!memory_buffer)
	{
		printk("error: allocating memory for the buffer\n");
		return -ENOMEM;
	}
	memset(memory_buffer, 0, BUF_SIZE);
	return 0;
}

static int my_driver_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "simple_buffer_release!\n");
	if (memory_buffer)
	{
		kfree(memory_buffer);
	}
	return 0;
}

static int my_driver_init(void)
{
	printk(KERN_INFO "my_river_init!\n");
	if (register_chrdev(DEV_MAJOR_NUMBER, DEV_NAME, &my_driver_fops) < 0) {
		printk(KERN_ERR "driver_exam register failed!\n");
		return -1;
	}


	return 0;
}

static void my_driver_exit(void)
{
	printk(KERN_INFO "my_driver_exit!\n");
	unregister_chrdev(DEV_MAJOR_NUMBER, DEV_NAME);
}

module_init(my_driver_init);
module_exit(my_driver_exit);
MODULE_LICENSE("GPL");