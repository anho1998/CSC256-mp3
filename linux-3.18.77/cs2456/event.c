#include <linux/kernel.h>
#include <linux/event.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/list.h>

// GLOBAL VARIABLE //
static int event_count; // number of current opened event
static int EID_next; // event ID of the next event
static LIST_HEAD(event_list_head); // list_head of all events
static DEFINE_MUTEX(event_lock); // mutex for event

void __init doevent_init(void)
{
	// intialization
	event_count = 0;
	EID_next = 0;	
	printk("EVENT INITIALIZATION SUCCEEDED\n");
}

// Function return the event from event ID
static struct event * get_event(int ID)
{
	struct event * temp;
	list_for_each_entry(temp, &event_list_head, event_list){
		if (temp->EID == ID)
			return temp;
	}
	
	return NULL;
}

asmlinkage long sys_doeventopen(void)
{
	struct event * new_event; 
	
	// Lock 
	mutex_lock(&event_lock);


	// Allocate memory
	new_event = (struct event *)kmalloc(sizeof(struct event), GFP_KERNEL);
	
	// If allocation unsuccessful 
	if (new_event == NULL)
		return -1;
	
	// Initilaize event info
	new_event->EID = EID_next;
	EID_next++;
	new_event->UID = sys_geteuid();
	new_event->GID = sys_getegid();
	new_event->user_enable = 1;
	new_event->group_enable = 1;
	new_event->event_sig = 1;
	init_waitqueue_head(&new_event->event_queue);
	INIT_LIST_HEAD(&new_event->event_list);
	list_add(&new_event->event_list, &event_list_head);
	event_count++;

	mutex_unlock(&event_lock);
	return new_event->EID;
}

asmlinkage long sys_doeventwait(int eventID)
{
	struct event * temp;
	uid_t pUID;
	gid_t pGID;

	mutex_lock(&event_lock);

	// Check if event ID exist
	temp = get_event(eventID);
	if (temp == NULL){
		mutex_unlock(&event_lock);
		printk("INVALID EVENT ID\n");
		return -1;
	}
	
	// Check for privilege
	pUID = sys_geteuid();
	pGID = sys_getegid();
	
	if (!((pUID == 0) | ((pUID == temp->UID) & (temp->user_enable == 1)) | ((pGID == temp->GID) & (temp->group_enable == 1)))){
		mutex_unlock(&event_lock);
		printk("NO PRIVILEGE\n");
		return -2;
	}
	
	//wait
	temp->event_sig = 0;
	mutex_unlock(&event_lock);
	wait_event_interruptible(temp->event_queue, temp->event_sig == 1);

	return 1;
}

asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag)
{
	struct event * temp;
	uid_t pUID;
	gid_t pGID;

	mutex_lock(&event_lock);

	// Check if event ID exist
	temp = get_event(eventID);
	if (temp == NULL){
		mutex_unlock(&event_lock);
		printk("INVALID EVENT ID\n");
		return -1;
	}	

	// Check for privilege
	pUID = sys_geteuid();
	pGID = sys_getegid();

	if (!((pUID==0) | (pUID == temp->UID))){
		mutex_unlock(&event_lock);
		printk("NO PRIVILEGE\n");
		return -2;
	}

	// Modify enable bits
	temp->user_enable = UIDFlag;
	temp->group_enable = GIDFlag;

	mutex_unlock(&event_lock);

	return 0;
}

