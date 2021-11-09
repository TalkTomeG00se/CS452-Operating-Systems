/*
 * booga.c -- the bare booga char module
 * This booga shows how to use a semaphore to avoid race conditions
 * in updating a global data structure inside a driver.
 */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/version.h> /* printk() */
#include <linux/init.h>  /*  for module_init and module_cleanup */
#include <linux/slab.h>  /*  for kmalloc/kfree */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>	/* for the proc filesystem */
#include <linux/seq_file.h>
#include "booga.h"        /* local definitions */
#include <linux/random.h> /* needed for random num generation */
#include <linux/string.h> /* needed for string usages */
#include <linux/sched/signal.h> /* needed for SIGTERM usage */



static int booga_major =   BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS;    /* number of bare booga devices */
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);
MODULE_AUTHOR("Colten Davis");
MODULE_LICENSE("GPL");

static booga_stats *booga_device_stats;
static struct proc_dir_entry* booga_proc_file;

static ssize_t booga_read (struct file *, char *, size_t , loff_t *);
static ssize_t booga_write (struct file *, const char *, size_t , loff_t *);
static int booga_open (struct inode *, struct file *);
static int booga_release (struct inode *, struct file *);
static int booga_proc_open(struct inode *inode, struct file *file);

/* keeps track of minor numbers used [0,1,2,3] */
static int numMinors;

char booga_String[] = "booga! booga!";
char nekas_String[] = "googoo! gaagaa!";
char googoos_String[] = "neka! maka!";
char woogas_String[] = "wooga! wooga!";

/*  The different file operations */
/* The syntax you see below is an extension to gcc. The prefered */
/* way to init structures is to use C99 Taged syntax */
/* static struct file_operations booga_fops = { */
/* 		    .read    =       booga_read, */
/* 			.write   =       booga_write, */
/* 			.open    =       booga_open, */
/* 			.release =       booga_release */
/* }; */
/*  This is where we define the standard read,write,open and release function */
/*  pointers that provide the drivers main functionality. */
static struct file_operations booga_fops = {
		    read:       booga_read,
			write:      booga_write,
			open:       booga_open,
			release:    booga_release,
};

/* The file operations struct holds pointers to functions that are defined by */
/* driver impl. to perform operations on the device. What operations are needed */
/* and what they should do is dependent on the purpose of the driver. */
static const struct file_operations booga_proc_fops = {
		.owner	= THIS_MODULE,
		.open	= booga_proc_open,
		.read	= seq_read,
		.llseek	= seq_lseek,
		.release	= single_release,
};


/*
 * Open and close
 */
static int booga_open (struct inode *inode, struct file *filp)
{
		int num = NUM(inode->i_rdev);

		if (num >= booga_nr_devs) return -ENODEV;

		filp->f_op = &booga_fops;

        numMinors = num; /* assigns the minor number chosen */

		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);

        if(num == 0){ /* booga0 */

            booga_device_stats->booga0_Opens++;

        } else if(num == 1){ /* booga1 */

            booga_device_stats->booga1_Opens++;

        } else if(num == 2){ /* booga2 */

            booga_device_stats->booga2_Opens++;

        } else if(num == 3){ /* booga3 */

            booga_device_stats->booga3_Opens++;

        }
		up(&booga_device_stats->sem);

		try_module_get(THIS_MODULE);
		return 0;          /* success */
}

static int booga_release (struct inode *inode, struct file *filp)
{
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);
		// booga_device_stats->num_close++;
		up(&booga_device_stats->sem);

		module_put(THIS_MODULE);
		return 0;
}

/*
 * Data management: read and write
 */

static ssize_t booga_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{       
        char* phrase;
        int minorNumUsed;
        int i = 0;
        char* temp = buf;
        char randomNum;
        get_random_bytes(&randomNum, 1);
        minorNumUsed = (randomNum & 0x7F) % 4;

		printk("<1>booga_read invoked.\n");
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);

        if(minorNumUsed == 0) { /* booga0 */

            phrase = booga_String;

            booga_device_stats->numBoogas++;

        } else if(minorNumUsed == 1){ /* booga1 */

            phrase = googoos_String;

            booga_device_stats->numGoogoos++;

        } else if(minorNumUsed == 2){ /* booga2 */

            phrase = nekas_String;

            booga_device_stats->numNekas++;

        } else if(minorNumUsed == 3){ /* booga3 */

            phrase = woogas_String;

            booga_device_stats->numWoogas++;

        }

        for(i = 0; i <= count; i += strlen(phrase)){

            if(i < count - strln(phrase)){

                strncpy(temp, phrase, strln(phrase));

                temp += strln(phrase);

            } else {

                strncpy(temp, phrase, count - i);

            }
        }
        
		booga_device_stats->numBytesRead += count;
		up(&booga_device_stats->sem);
		return count;
}

static ssize_t booga_write (struct file *filp, const char *buf, size_t count , loff_t *f_pos)
{
		printk("<1>booga_write invoked.\n");/* need to protect this with a semaphore if multiple processes will invoke this driver to prevent a race condition */

        //struct pid* pid; /* struct for the process ID, will kill the process it holds */

        if(numMinors == 3){ // per project, if process attempts to write to booga3

            //pid = get_task_pid(current, PIDTYPE_PID); // get PID of current task

            kill_pid(get_task_pid(current, PIDTYPE_PID), SIGTERM, 1); // kill with SIGTERM per project specs

            return 0;
        }

		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);
		booga_device_stats->numBytesWrite += count;
		up(&booga_device_stats->sem);
		return count; // pretend that count bytes were written
}

static void init_booga_device_stats(void)
{
	booga_device_stats->booga0_Opens = 0;
    booga_device_stats->booga1_Opens = 0;
    booga_device_stats->booga2_Opens = 0;
    booga_device_stats->booga3_Opens = 0;
    booga_device_stats->numBytesRead = 0;
    booga_device_stats->numBytesWrite = 0;
    booga_device_stats->numBoogas = 0;
    booga_device_stats->numNekas = 0;
    booga_device_stats->numGoogoos = 0;
    booga_device_stats->numWoogas = 0;
	sema_init(&booga_device_stats->sem, 1);
}

static int booga_proc_show(struct seq_file *m, void *v)
{
		seq_printf(m, "bytes read = %ld\n", booga_device_stats->numBytesRead);
		seq_printf(m, "bytes written = %ld\n", booga_device_stats->numBytesWrite);
		seq_printf(m, "number of opens:\n");
		seq_printf(m, "\t/dev/booga0 = %ld times\n", booga_device_stats->booga0_Opens);
		seq_printf(m, "\t/dev/booga1 = %ld times\n", booga_device_stats->booga1_Opens);
		seq_printf(m, "\t/dev/booga2 = %ld times\n", booga_device_stats->booga2_Opens);
		seq_printf(m, "\t/dev/booga3 = %ld times\n", booga_device_stats->booga3_Opens);
		seq_printf(m, "strings output:\n");
		seq_printf(m, "\tbooga! booga!  = %ld times\n", booga_device_stats->numBoogas);
		seq_printf(m, "\tgoogoo! gaagaa!  = %ld times\n", booga_device_stats->numGoogoos);
		seq_printf(m, "\tneka! maka!  = %ld times\n", booga_device_stats->numNekas);
		seq_printf(m, "\twooga! wooga!  = %ld times\n", booga_device_stats->numWoogas);
		return 0;
}


static int booga_proc_open(struct inode *inode, struct file *file)
{
		return single_open(file, booga_proc_show, NULL);
}


static __init int booga_init(void)
{
		int result;

		/*
		 * Register your major, and accept a dynamic number
		 */
		result = register_chrdev(booga_major, "booga", &booga_fops);
		if (result < 0) {
				printk(KERN_WARNING "booga: can't get major %d\n",booga_major);
				return result;
		}
		if (booga_major == 0) booga_major = result; /* dynamic */
		printk("<1> booga device driver version 4: loaded at major number %d\n", booga_major);

		booga_device_stats = (booga_stats *) kmalloc(sizeof(booga_stats),GFP_KERNEL);
		if (!booga_device_stats) {
				result = -ENOMEM;
				goto fail_malloc;
		}
		init_booga_device_stats();

		/* We assume that the /proc/driver exists. Otherwise we need to use proc_mkdir to
		 * create it as follows: proc_mkdir("driver", NULL);
		 */
		booga_proc_file = proc_create("driver/booga", 0, NULL, &booga_proc_fops);
		if (!booga_proc_file)  {
				result = -ENOMEM;
				goto fail_malloc;
		}

		return 0;

fail_malloc:
		unregister_chrdev(booga_major, "booga");
		return  result;
}



static __exit  void booga_cleanup(void)
{
		remove_proc_entry("driver/booga", NULL /* parent dir */);
		kfree(booga_device_stats);
		unregister_chrdev(booga_major, "booga");
		printk("<1> booga device driver version 4: unloaded\n");
}


module_init(booga_init);
module_exit(booga_cleanup);

/* vim: set ts=4: */