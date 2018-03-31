#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61

int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void onebyte_exit(void);

struct file_operations onebyte_fops = {
     read: onebyte_read,
     write: onebyte_write,
     open: onebyte_open,
     release: onebyte_release
};

char *onebyte_data = NULL;

int onebyte_open(struct inode *inode, struct file *filep)
{
     return 0;
}
int onebyte_release(struct inode *inode, struct file *filep)
{
     return 0;
}
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
     int bytes_read = 0;
     
     if(*buf != 0){
          printk(KERN_ALERT "READ FAILED: char buffer not empty!\n");
          return 0; 
     }

     copy_to_user(buf, onebyte_data, sizeof(char));
     bytes_read ++;
     printk(KERN_ALERT "READ SUCCESS: %c\n", *onebyte_data);
     return bytes_read;
}
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
     int bytes_write = 0;     

     if(count > sizeof(char))
     {
          printk(KERN_ALERT "WRITE FAILED: No space left on device\n");
          return -ENOSPC; 
     }

     copy_from_user(onebyte_data, buf, sizeof(char));
     bytes_write ++;
     printk(KERN_ALERT "WRITE SUCCESS: %c\n", *onebyte_data);
     return bytes_write;
}

static int onebyte_init(void)
{
     int result;
     result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
     if (result < 0) {
          return result;
     }
     onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);

     if (!onebyte_data) {
          onebyte_exit();
          return -ENOMEM;
     }

     *onebyte_data = 'X';

     printk(KERN_ALERT "This is a onebyte device module\n");
     return 0;
}

static void onebyte_exit(void)
{
     if (onebyte_data) {
          kfree(onebyte_data);
          onebyte_data = NULL;
     }

     unregister_chrdev(MAJOR_NUMBER, "onebyte");
     printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);