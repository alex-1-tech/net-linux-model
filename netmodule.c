#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/etherdevice.h>
#include <linux/netdevice.h>

static int net_module_open(struct net_device *dev) {
  pr_info("netmodule: open\n");
  netif_start_queue(dev);
  return 0;
}

static int net_module_stop(struct net_device *dev) {
  pr_info("netmodule: stop\n");
  netif_stop_queue(dev);
  return 0;
}

static netdev_tx_t net_module_xmit(struct sk_buff *skb,
                                   struct net_device *dev) {
  pr_info_ratelimited("netmodule: packet xmit, len=%u\n", skb->len);

  dev->stats.tx_packets++;
  dev->stats.tx_bytes += skb->len;

  dev_consume_skb_any(skb);

  return NETDEV_TX_OK;
}

static const struct net_device_ops net_module_netdev_ops = {
    .ndo_open = net_module_open,
    .ndo_stop = net_module_stop,
    .ndo_start_xmit = net_module_xmit,
};

struct net_device *net_dev;
static int __init net_module_init(void) {
  pr_info("init");

  int err;

  net_dev = alloc_netdev(0, "netmodule%d", NET_NAME_UNKNOWN, ether_setup);
  if (!net_dev)
    return -ENOMEM;

  net_dev->netdev_ops = &net_module_netdev_ops;
  eth_hw_addr_random(net_dev);

  err = register_netdev(net_dev);
  if (err) {
    free_netdev(net_dev);
    net_dev = NULL;
    return err;
  }

  pr_info("Created interface: %s\n", net_dev->name);
  return 0;
}
static void __exit net_module_exit(void) {
  if (net_dev) {
    unregister_netdev(net_dev);
    free_netdev(net_dev);
    net_dev = NULL;
  }
}

module_init(net_module_init);
module_exit(net_module_exit);

MODULE_LICENSE("BSD 3-Clause");
MODULE_AUTHOR("alex-1-tech");
MODULE_DESCRIPTION("Linux net module");