/*
 * dummy driver for lab5
 * 
 * Adopted from 'Writing Device Drivers in Linux'
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include "uaccess.h" /* copy_from/to_user */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>


MODULE_LICENSE("Dual BSD/GPL");

/* Necessary structures */
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

/* Declaration of dummy.c functions */
int etx_open(struct inode *inode, struct file *filp);
int etx_release(struct inode *inode, struct file *filp);
ssize_t etx_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t etx_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void etx_exit(void);
int etx_init(void);

/* access functions */
struct file_operations fops = {
  read: etx_read,
  write: etx_write,
  open: etx_open,
  release: etx_release
};

/* Declaration of the init and exit functions */
module_init(etx_init);
module_exit(etx_exit);


/* Global variables of the driver */
/* Fixed Major number */
int memory_major = 160;

/* Buffer to store data */
char *memory_buffer;

/*
 * Code to run on insert of driver to kernel 
 * This should allocate free major device number 
 */
int etx_init(void) {

	/* Allocating Major number */
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0){
		printk("<1>dummy: cannot allocate major number\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&etx_cdev, &fops);

	/* Adding character device to the system */
	if((cdev_add(&etx_cdev,dev,1)) < 0){
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_class;
	}

	/* Creating struct class */
	if((dev_class = class_create("etx_class")) == NULL){
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/* Creating device */
	if((device_create(dev_class,NULL,dev,NULL, "etx_device")) == NULL){
		printk(KERN_INFO "Cannot create the device 1\n");
		goto r_device;
	}

	  /* Now initialise the buffer for the dummy */
	  int result;

	  /* Allocating memory for the buffer */
	  memory_buffer = kmalloc(5, GFP_KERNEL); 
		  if (!memory_buffer) { 
	    result = -ENOMEM;
	    goto fail; 
	  } 

	  memset(memory_buffer, 0, 5);

        printk(KERN_INFO "dummy driver: Device Driver Insert...Done!!!\n");
        return 0;

fail: 
    printk(KERN_INFO "dummy driver: Device Driver insert FAILED no memory\n");
    etx_exit(); 
    return result;

r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

/*
 * exit module called on removing the module from the kernel
 */
void etx_exit(void) {

  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&etx_cdev);
  unregister_chrdev_region(dev, 1);

  /* Freeing buffer memory */
  if (memory_buffer) {
    kfree(memory_buffer);
  }

  printk(KERN_INFO "dummy driver: Device Driver Remove...Done!!!\n");

}

/*
 * Called on the open of the driver
 */
int etx_open(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "dummy driver: Device Driver open() called\n");
  return 0;
}

/*
 * Called on the close of the driver
 */
int etx_release(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "dummy driver: Device Driver release() called\n");
  return 0;
}

/*
 * Called on the read of the driver
 */
ssize_t etx_read(struct file *filp, char __user *buf, 
                    size_t count, loff_t *f_pos) { 

  printk(KERN_INFO "dummy driver: Device Driver read() called\n");
  printk(KERN_INFO "dummy driver: buffer = [%x]\n", memory_buffer[0]);
  /* Transfering data to user space */ 
  copy_to_user((char *)buf,memory_buffer,5);

  /* Changing reading position as best suits */ 
  if (*f_pos == 0) { 
    *f_pos+=5; 
    return 5; 
  } else { 
    return 0; 
  }
}

/*
 * Called on the write to the driver
 */
ssize_t etx_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {

  char *tmp;

  printk(KERN_INFO "dummy driver: Device Driver write() called\n");

  tmp=(char *)buf+count-5;
  copy_from_user(memory_buffer,tmp,5);
  return 5;
}

/* End of driver */ 
