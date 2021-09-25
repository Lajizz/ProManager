#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/export.h>
#include "netlink.c"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lajizz");



// void walk_page_table(unsigned long addr)
// {
//     pgd_t *pgd =(pgd_t *) kallsyms_lookup_name("swapper_pg_dir");
//     //pgd_t *pgd = pgd_offset_k(addr);
//     p4d_t *p4d = NULL;
//     pud_t *pud = NULL;
//     pmd_t *pmd = NULL;
//     pte_t *pte = NULL;
    
    
//     //pgd = pgd_offset_k(addr));
//     if (pgd_none(*pgd) || pgd_bad(*pgd)){
//         printk(KERN_INFO "Valid pgd\n");
//         return;
//     }
//     printk(KERN_INFO "pgd 0x%lx\n", *pgd);
//     p4d = p4d_offset(pgd, addr);
//     if(p4d_none(*p4d) || p4d_bad(*p4d)){
//         printk(KERN_INFO "Valid p4d\n");
//         return;
//     }
        
//     pud = pud_offset(p4d, addr);
//     if (pud_none(*pud) || pud_bad(*pud)){
//         printk(KERN_INFO "Valid pud\n");
//         return;
//     }
//     printk(KERN_INFO "pud 0x%lx\n", *pud);

//     pmd = pmd_offset(pud, addr);
//     if (pmd_none(*pmd) || pmd_bad(*pmd)){
//         printk(KERN_INFO "Valid pmd\n");
//         return;
//     }
        
//     printk(KERN_INFO "pmd 0x%lx\n", *pmd);
//     pte = pte_offset_map(pmd, addr);
//     //*pte = clear_pte_bit(*pte, __pgprot((_AT(pteval_t, 1) << 7)));

//     // if(pgd_none(*pgd) || pgd_bad(*pgd))
//     //     return NULL;

//     // pud = pud_offset(p4d, addr);
//     // if(pud_none(*pud) || pud_bad(*pud))
//     //     return NULL;
//     // pmd = pmd_offset(pud, addr);
//     // if(pmd_none(*pmd) || pmd_bad(*pmd))
//     //     return NULL;
//     // pte = pte_offset_map(pmd, addr);
//     // if(pte_none(*pte) || !pte_present(*pte))

//     //     return NULL;
//     printk(KERN_INFO "table_pte: %x \n",pte_write(*pte));

//     //table_pte = *pte;
//     //printk(KERN_INFO "table_pte: %x %d\n", table_pte,pte_write(table_pte));
    




//     // printk(KERN_INFO "PTE before 0x%lx\n", pte);
//     // printk(KERN_INFO "Setting PTE write\n");
//     // pte = pte_mkwrite(pte);
//     // printk(KERN_INFO "PTE after         0x%lx\n", pte);

//  }



void set_addr_rw(unsigned long addr) {

    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    printk(KERN_INFO"pte:%d\n",pte);
    printk(KERN_INFO"can write:%d\n",pte_write(*pte));
    pte_t temp = pte_mkwrite(*pte);
    //pte_t *pte = lookup_address(addr, &level);
    printk(KERN_INFO"can write:%d\n",pte_write(temp));
    //if (pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;

}

void set_addr_ro(unsigned long addr) {

    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    printk(KERN_INFO"can write:%d\n",pte_write(*pte));
    pte_wrprotect(*pte);
    //pte_t *pte = lookup_address(addr, &level);
    printk(KERN_INFO"can write:%d\n",pte_write(*pte));
    //pte->pte = pte->pte &~_PAGE_RW;

}


typedef long(sys_clone_sig)(unsigned long, unsigned long, int __user *,
                            int __user *, unsigned long);


asmlinkage sys_clone_sig *original_sys_clone = NULL;

unsigned long *original_syscall_table = NULL;

//reload sys_read
typedef asmlinkage ssize_t (*old_syscall_t)(struct pt_regs* regs);
old_syscall_t old_read = NULL;



unsigned long *obtain_syscall_table_bf(void)
{
    /* get sys_call_table by kernel symbols */
    unsigned long i = kallsyms_lookup_name("sys_call_table");
    if (i == 0)
    {
        return NULL;
    }
    return (unsigned long *)i;
}


asmlinkage int hooked_sys_clone(unsigned long x1,
                                unsigned long x2,
                                int __user *x3,
                                int __user *x4,
                                unsigned long x5)
{
    int ret_val = original_sys_clone(x1, x2, x3, x4, x5);
    //printk(KERN_INFO "hook success\n");
    return ret_val;
}

asmlinkage ssize_t hooked_read(struct pt_regs* regs) {
    ssize_t ret = -1;
    if(current->pid == pid){
          printk(KERN_INFO"READ_HOOKED");
    }
    ret = old_read(regs);
  
    return ret;
}
static int __init syscall_hook_init(void)
{
    unsigned int l;
    
    printk(KERN_INFO "insert hook module\n");
    int i = init_netlink();
    if(i != 0){
        return -1;
    }
    original_syscall_table = obtain_syscall_table_bf();
    printk(KERN_INFO "syscall table: %px\n", original_syscall_table);
    printk(KERN_INFO "syscall clone: %px\n", (unsigned long)&original_syscall_table[__NR_clone]);
    //walk_page_table((unsigned long)&original_syscall_table[__NR_clone]);
    //table_pte = lookup_address((unsigned long)&original_syscall_table[__NR_clone],&i);
    pte_t* pte = lookup_address((long unsigned int)original_syscall_table,&l);  
    printk(KERN_INFO "pte :%x\n",*pte);
    pte->pte |= _PAGE_RW;
    //printk(KERN_INFO"write bit:%d %d\n",pte_write(*table_pte),__NR_clone);

    //pte_t temp1 = pte_mkwrite(*table_pte);
    //printk(KERN_INFO"can write:%d\n",pte_write(temp1));
    original_sys_clone = (sys_clone_sig *)original_syscall_table[__NR_clone];
    original_syscall_table[__NR_clone] = (unsigned long)hooked_sys_clone;
    old_read = (old_syscall_t) original_syscall_table[__NR_read];
    original_syscall_table[__NR_read] = (unsigned long)hooked_read;

    //pte_t temp2 = pte_wrprotect(temp1);
    //printk(KERN_INFO"write bit:%d\n",pte_write(temp2));

    return 0;
}
static void __exit syscall_hook_exit(void)
{
    unsigned int l;
    //pte_mkwrite(*table_pte);
    original_syscall_table[__NR_clone] = (unsigned long)original_sys_clone;
    original_syscall_table[__NR_read] = (unsigned long)old_read;
    
    pte_t* pte = lookup_address((long unsigned int)original_syscall_table,&l);
    pte->pte &= ~_PAGE_RW;

    //pte_wrprotect(*table_pte);
    remove_netlink();
    printk(KERN_INFO "rm hook module\n");
}

module_init(syscall_hook_init);
module_exit(syscall_hook_exit);