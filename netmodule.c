#include <linux/module.h>
#include <linux/kernel.h>

static int __init net_module_init(void){
    printk(KERN_INFO "init\n");
    return 0;
} 
static void __exit net_module_exit(void)
{
    printk(KERN_INFO "exit\n");
}

module_init(net_module_init);
module_exit(net_module_exit);

MODULE_LICENSE("BSD 3-Clause");
MODULE_AUTHOR("alex-1-tech");
MODULE_DESCRIPTION("Linux net module");