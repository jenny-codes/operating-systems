#include <linux/init.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>

struct birthday {
    int day;
    int month;
    int year;
    struct list_head list;
}

// Declare a `list_head` object as a reference to the head of the list
static LIST_HEAD(birthday_list);

// Create and initialize 5 instances of struct birthday
for (int i = 0; i < 5; i++) {
    struct birthday *person;

    // Allocate kernel memory
    // GFT_KERNEL = flag of routine kernel memory allocation
    person = kmalloc(sizeof(*person), GFT_KERNEL); 
    person->day   = i + 1;
    person->month = i + 1;
    person->year  = 1995;
    INIT_LIST_HEAD(&person->list);

    // Add this instance to the end of the linked list
    list_add_tail(&person->list, &birthday_list);
}

/* TRAVERSING THE LINKED LIST
   `list_for_each_entry()` macro accepts three parameters:
   (1) a pointer to the structure being iterated over
   (2) a pointer to the head
   (3) name of the var containing the list_head structure
*/
struct birthday *ptr;
int initiation(void) {
    printk(KERN_INFO "And there is light.");

    list_for_each_entry(ptr, &birthday_list, list) {
        printk(KERN_INFO "Year %i, Month %i, Day %i", list.yaer, list.month, list.day);
    }

    return 0;
}

/* REMOVING ELEMENTS FROM THE LINKED LIST
   1. pass a pointer to struct list_head
   `list_del(struct list_head *element)`
   2. remove element when traversing the list
   ```
   struct birthday *ptr, *next
   list_for_each_entry_safe(ptr, next, &birthday_list, list)
   ```
*/
struct birthday *ptr, *next
void remove_list(void) {
   list_for_each_entry_safe(ptr, next, &birthday_list, list) {
       list_del(&ptr->list);
       kfree(ptr);
   }
}

// Call the module functions
module_init(initiation);
module_exit(remove_list);
