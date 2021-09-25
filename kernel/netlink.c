#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/netlink.h>
#include<linux/types.h>
#include<net/sock.h>
#include<linux/string.h>
//#include<lib/vsprintf.c>
#define NETLINK_TEST 30
#define MSG_LEN 125
#define USER_PORT 100


struct sock *nlsk = NULL;
extern struct net init_net;

unsigned int PolicyArray[10];
int pid = -1;

int string2int(const char* str){
    int array[10] = {0,1,2,3,4,5,6,7,8,9};
    int i = 0;
    int res = 0;
    for(i= 0;i<strlen(str);i++){
        printk("number:%d,%c\n",res,str[i]);
        res = res*10 + (str[i]-'0');
    }
    return res;
}
static void netlink_rcv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *umsg = NULL;
    char *kmsg = "hello users!!!";
    printk(KERN_INFO"RCV\n");
    printk(KERN_INFO"skb->len: %d\n",skb->len);
    if(skb->len >= nlmsg_total_size(0))
    {
        printk(KERN_INFO"RCV2\n");
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if(umsg)
        {
            
            printk(KERN_INFO"kernel recv from user: %s\n", umsg);
            pid = string2int(umsg);
            printk(KERN_INFO"the process in policy:%d\n",pid);
            //send_usrmsg(kmsg, strlen(kmsg));
        }
    }
}

struct netlink_kernel_cfg cfg = {
    .input = netlink_rcv_msg,
};
int init_netlink(void){
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if(nlsk == NULL)
    {   
        printk("netlink_kernel_create error !\n");
        return -1; 
    }
    //init for array   
    int i;
    for(i = 0;i<10;i++){
        PolicyArray[i] = 0;
    }
    printk("test_netlink_init\n");
    return 0;
}

void remove_netlink(void){
    if (nlsk){
        netlink_kernel_release(nlsk); /* release ..*/
        nlsk = NULL;
    }
    printk(KERN_INFO"release\n");

}
