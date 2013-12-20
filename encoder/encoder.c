#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/param.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <linux/time.h>
#include <linux/proc_fs.h>

#define AM33XX_CONTROL_BASE 0x44e10000
#define INPIN (AM33XX_CONTROL_BASE+0x840)
#define INPIN_MODE (0x7|(2<<3)|(1<<5))
#define procfs_name "treadmill_speed"

static u16 in_pin;
static u16 irq;
static s64 last_time = 0;
static s64 speed = 0;
struct proc_dir_entry *Our_Proc_File;



static irqreturn_t encoder_interrupt_handler(int irq, void* dev_id)
{
  struct timespec temp = current_kernel_time();
  s64 new_time = timespec_to_ns(&temp);
  s64 time_diff = (new_time - last_time);
  if(time_diff > 1000000) // Discard interrupts faster than 1ms
  {
    speed = time_diff;
  }
  printk(KERN_INFO "Time since last interrupt %lld.!\n", (long long) time_diff);
  last_time = new_time;
  return IRQ_HANDLED;
}



int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);

	if (offset > 0) {
		ret  = 0;
	} else {
		ret = sprintf(buffer, "%lld\n", (long long) speed);
	}

	return ret;
}



static int __init hello_start(void)
{
  void* addr = ioremap(INPIN, 4);
  
  if(NULL == addr)
    return -EBUSY;

  iowrite32(INPIN_MODE, addr);
  iounmap(addr);

  in_pin = 48;

  int err = gpio_request_one(in_pin, GPIOF_IN, "Button_isr");
  if(err < 0)
  {
    printk(KERN_ALERT "Failed to request input pin %d.\n", in_pin);
    return -1;
  }

  err = gpio_to_irq(in_pin);
  if(err < 0)
  {
    printk(KERN_ALERT "Failed to get IRQ for pin %d.\n", in_pin);
    goto err_free_gpio_return;
  }
  else
  {
    irq = (u16)err;
  }

  err = request_any_context_irq(irq, encoder_interrupt_handler, IRQF_TRIGGER_FALLING | IRQF_DISABLED, "Button_isr", (void*) NULL);

  if(err < 0)
  {
    printk(KERN_ALERT "Failed to enable IRQ %d for pin %d.\n", irq, in_pin);
    goto err_free_gpio_return;
  }

  Our_Proc_File = create_proc_entry(procfs_name, 0644, NULL);
	
	if (Our_Proc_File == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
		       procfs_name);
		return -ENOMEM;
	}

	Our_Proc_File->read_proc = procfile_read;
	Our_Proc_File->mode = S_IFREG | S_IRUGO;
	Our_Proc_File->uid = 0;
	Our_Proc_File->gid = 0;
	Our_Proc_File->size = 37;

	printk(KERN_INFO "Encoder driver loaded\n");	
  
  return 0;

  err_free_gpio_return:
  gpio_free(in_pin);
  return err;
}



static void __exit hello_end(void)
{
  free_irq(irq, (void*) NULL);
  gpio_free(in_pin);
	remove_proc_entry(procfs_name, NULL);
  printk(KERN_INFO "Encoder driver unloaded\n");
}

module_init(hello_start);
module_exit(hello_end);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Kernel module to read from GPIO pin");
MODULE_VERSION("1.0");
