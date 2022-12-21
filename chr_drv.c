#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define mem_size 1024

dev_t dev = 0;
static struct cdev my_cdev;
static struct class *dev_class;
uint8_t *kernel_buffer;


static int __init chr_driver_init(void);
static void __exit cht_driver_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t *off);



static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
};

static int my_open(struct inode *inode, struct file *file)
{
	//create memory
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO "Can not allocate memory");
		return -1;
	}
	printk(KERN_INFO "Memory allocated..");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO "Closing device");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	copy_to_user(buf, kernel_buffer, mem_size);
	printk(KERN_INFO "data read");
	return mem_size;
}

static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	copy_from_user(kernel_buffer, buf, len);
	printk(KERN_INFO "data write");
	return len;
}



static int __init chr_driver_init(void)
{
	//allocat major and minor number
	//alloc_chrdev_region(dev_t *dev, firstminor, count, name)
	if((alloc_chrdev_region(&dev, 0, 1, "my_dev")) < 0) {
		printk(KERN_INFO "Unable to allocate major and minor number");
		return -1;
	}
	printk(KERN_INFO "Major no. %d, Minor no. %d", MAJOR(dev), MINOR(dev));

	//create cdev structure 
	cdev_init(&my_cdev, &fops);

	//Add device to system
	//cdev_add
	if((cdev_add(&my_cdev, dev, 1))<0)
	{
		printk(KERN_INFO "Unable to add device to system");
		goto r_class;
	}

	//create class
	if((dev_class = class_create(THIS_MODULE, "my_class")) == NULL)
	{
		printk(KERN_INFO "Unable to create class");
		goto r_class;
	}

	//create device 
	if((device_create(dev_class, NULL, dev, NULL, "My_Device")) == NULL)
	{
		printk(KERN_INFO "Unable to allocate device");
		goto r_device;
	}
	printk(KERN_INFO "Device driver insert....done");
	return 0;



r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
r_device:
	class_destroy(dev_class);
	return -1;
}

void __exit chr_driver_exit(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver removed!");
	return;
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nishant Thummar");
MODULE_DESCRIPTION("A Simple Character Device Driver");
