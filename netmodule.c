#include <linux/etherdevice.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/rtnetlink.h>
#include <linux/uaccess.h>

#define MOD_NAME "netnet"
#define PROC_FILENAME "netmodule_ip"

static struct net_device *ndev;
static int set_ip_via_cmd(const char *ip) {
  char ip_cidr[64];
  char *argv[] = {"/sbin/ip", "addr", "add", ip_cidr, "dev", NULL, NULL};
  char *envp[] = {"HOME=/", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL};

  if (!ndev)
    return -ENODEV;

  argv[5] = ndev->name;
  snprintf(ip_cidr, sizeof(ip_cidr), "%s/24", ip);

  pr_info("%s: running 'ip addr add %s dev %s'\n", MOD_NAME, ip_cidr,
          ndev->name);
  return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

static ssize_t net_proc_write(struct file *file, const char __user *buffer,
                              size_t count, loff_t *ppos) {
  char k_buf[32];
  char *clean_ip;

  if (count >= sizeof(k_buf))
    return -EINVAL;

  if (copy_from_user(k_buf, buffer, count))
    return -EFAULT;

  k_buf[count] = '\0';
  clean_ip = strim(k_buf);

  if (set_ip_via_cmd(clean_ip))
    return -EIO;

  return count;
}

static netdev_tx_t net_xmit(struct sk_buff *skb, struct net_device *dev) {
  struct sk_buff *skb_copy;

  dev->stats.tx_packets++;
  dev->stats.tx_bytes += skb->len;

  skb_copy = skb_clone(skb, GFP_ATOMIC);
  dev_kfree_skb(skb);

  if (!skb_copy)
    return NETDEV_TX_OK;

  skb_copy->dev = dev;
  skb_copy->protocol = eth_type_trans(skb_copy, dev);
  skb_copy->ip_summed = CHECKSUM_UNNECESSARY;

  dev->stats.rx_packets++;
  dev->stats.rx_bytes += skb_copy->len;

  netif_rx(skb_copy);
  return NETDEV_TX_OK;
}

static const struct net_device_ops net_netdev_ops = {
    .ndo_open = (void *)eth_validate_addr,
    .ndo_start_xmit = net_xmit,
    .ndo_set_mac_address = eth_mac_addr,
    .ndo_validate_addr = eth_validate_addr,
};

static void net_setup(struct net_device *dev) {
  ether_setup(dev);
  dev->netdev_ops = &net_netdev_ops;
  dev->flags |= IFF_NOARP;
  eth_hw_addr_random(dev);
}

static const struct proc_ops net_pops = {
    .proc_write = net_proc_write,
};

static int __init net_mod_init(void) {
  int res;

  ndev = alloc_netdev(0, "netmodule%d", NET_NAME_UNKNOWN, net_setup);
  if (!ndev)
    return -ENOMEM;

  res = register_netdev(ndev);
  if (res) {
    pr_err("%s: failed to register device\n", MOD_NAME);
    free_netdev(ndev);
    return res;
  }

  if (!proc_create(PROC_FILENAME, 0666, NULL, &net_pops)) {
    unregister_netdev(ndev);
    free_netdev(ndev);
    return -ENOMEM;
  }

  pr_info("%s: loaded, dev %s created\n", MOD_NAME, ndev->name);
  return 0;
}

static void __exit net_mod_exit(void) {
  remove_proc_entry(PROC_FILENAME, NULL);
  if (ndev) {
    unregister_netdev(ndev);
    free_netdev(ndev);
  }
  pr_info("%s: unloaded\n", MOD_NAME);
}

module_init(net_mod_init);
module_exit(net_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex <alex-1-tech>");
MODULE_DESCRIPTION("Linux net module");