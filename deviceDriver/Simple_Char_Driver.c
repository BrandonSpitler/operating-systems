#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<asm/uaccess.h>
#define BUFFER_SIZE 1024
MODULE_LICENSE=("GPL");
static int timesOpened=0;
static int timesClosed=0;

static char device_buffer[BUFFER_SIZE];

ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/*  length is the length of the userspace buffer*/
	/*  current position of the opened file*/
	/* copy_to_user function. source is device_buffer (the buffer defined at the start of the code) and destination is the userspace 		buffer *buffer */


	int bytes_read =strlen(buff_rptr);
	if(bytes_read>length){
		bytes_read = length;
	}
	copy_to_user(buffer, buff_rptr,bytes_read);
	buff_rptr += bytes_read;
	return bytes_read;

	return 0;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/*  length is the length of the userspace buffer*/
	/*  current position of the opened file*/
	/* copy_from_user function. destination is device_buffer (the buffer defined at the start of the code) and source is the userspace 		buffer *buffer */

	int bytes_written= BUFFER_SIZE-(buff_wptr-device_buffer);
	if(bytes_written >length){
		bytes_written=length;
	}	
	printk(KERN_ALERT "iNSIDE OF WRITE %s\n", buffer);	
	copy_from_user(buff_wptr,buffer,bytes_written);
	buff_wptr+=strlen(buffer);
	return bytes_written;

}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{	
	timesOpened++;
	printk(KERN_ALERT "Device opened %d times\n",timesOpened);
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	return 0;
}


int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	timesClosed++;
	printk(KERN_ALERT "Device closed %d times\n",timesClosed);
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	return 0;
}

struct file_operations simple_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.open    = simple_char_driver_open,
	.write	 = simple_char_driver_write,
	.release = simple_char_driver_close,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int simple_char_driver_init(void)
{
	
	int t= register_chrdev(255,"simple_character_device",&file_operations simple_char_driver_file_operations);
	if(t<0){
		printk(KERN_ALERT "Device faiiled to register\n");
	}else{
		printk(KERN_ALERT "Device registered \n");
	}

	/* print to the log file that the init function is called.*/
	/* register the device */
	return 0;
}

static int simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	unregister_chrdev(255,"simple_character_device")
	/* unregister  the device using the register_chrdev() function. */
	return 0;
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);
