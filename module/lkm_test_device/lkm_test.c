#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

#ifndef LKM_TEST_MAJOR
#define LKM_TEST_MAJOR 200 /* ds18b20主设备号 */
#endif

#ifndef LKM_TEST_NR_DEVS
#define LKM_TEST_NR_DEVS 1 /* 设备数 */
#endif

#define LKM_TEST_DEV_NAME "LKM_TEST"

struct class *lkm_test_class;
static int lkm_test_major = LKM_TEST_MAJOR;
static struct cdev lkm_test_dev;
static unsigned short lkm_test_buf[1024];
static unsigned long lkm_test_len;

static int lkm_test_open(struct inode *inode, struct file *filp);
static int lkm_test_release(struct inode *inode, struct file *filp);
static int lkm_test_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops);
static int lkm_test_write(struct file *filp, char const __user *buf, size_t count, loff_t *f_ops);

static struct file_operations lkm_test_fops = {
	.owner = THIS_MODULE,
	.open = lkm_test_open,
	.release = lkm_test_release,
	.read = lkm_test_read,
	.write = lkm_test_write,
};

static int __init lkm_test_init(void)
{
    int ret;
	struct device *device;

	dev_t devno = MKDEV(lkm_test_major, 0);
	printk("lkm_test init...\n");
	/* 分配设备号 */
	if(lkm_test_major){
		ret = register_chrdev_region(devno, LKM_TEST_NR_DEVS, LKM_TEST_DEV_NAME);
	}else{
		ret = alloc_chrdev_region(&devno, 0, LKM_TEST_NR_DEVS, LKM_TEST_DEV_NAME);
		lkm_test_major = MAJOR(devno);
	}
	if(ret < 0){
		return ret;
	}
	
	/* 初始化并注册cdev结构 */
	lkm_test_dev.owner = THIS_MODULE;
	cdev_init(&lkm_test_dev, &lkm_test_fops);
	ret = cdev_add(&lkm_test_dev, MKDEV(lkm_test_major, 0), LKM_TEST_NR_DEVS);
	if(ret){
		printk("cdev add failed\n");
		unregister_chrdev_region(devno, LKM_TEST_NR_DEVS);
		return ret;
	}

	/* 自动创建设备文件 */
	lkm_test_class = class_create(THIS_MODULE, LKM_TEST_DEV_NAME);
	if(IS_ERR(lkm_test_class)){
		printk("class create failed\n");
		ret = PTR_ERR(lkm_test_class);
		unregister_chrdev_region(devno, LKM_TEST_NR_DEVS);
		return ret;
	}
	device = device_create(lkm_test_class, NULL, devno, NULL, LKM_TEST_DEV_NAME);
	if(IS_ERR(device)){
		printk("device create failed\n");
		class_destroy(lkm_test_class);
		unregister_chrdev_region(devno, LKM_TEST_NR_DEVS);
	}
    lkm_test_len = 0;
    memset(lkm_test_buf, 0, sizeof(lkm_test_buf));
	return 0;
}
static void __exit lkm_test_exit(void)
{
	printk("lkm_test exit...\n");
	device_destroy(lkm_test_class, MKDEV(lkm_test_major, 0));
	class_destroy(lkm_test_class);
	cdev_del(&lkm_test_dev);
	unregister_chrdev_region(MKDEV(lkm_test_major, 0), LKM_TEST_NR_DEVS);
}

static int lkm_test_open(struct inode *inode, struct file *filp)
{
	return 0;
}
static int lkm_test_release(struct inode *inode, struct file *filp)
{
	return 0;
}
static int lkm_test_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
    unsigned long ret, plen;

    plen = count>lkm_test_len?lkm_test_len:count;
    ret = copy_to_user(buf, lkm_test_buf, plen);
    return ret?-EFAULT:plen;
}
static int lkm_test_write(struct file *filp, char const __user *buf, size_t count, loff_t *f_ops)
{
    unsigned long ret, plen;

    plen = count>sizeof(lkm_test_buf)?sizeof(lkm_test_buf):count;
    ret = copy_from_user(lkm_test_buf, buf, plen);
    lkm_test_len = plen;
    return ret?-EFAULT:plen;
}

module_init(lkm_test_init);
module_exit(lkm_test_exit);

MODULE_LICENSE("GPL");
