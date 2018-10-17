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
// Unblocks all waiting processes; ignored if no processes 
// are blocked. Return number of processes signaled on 
// success and -1 on failure.
asmlinkage long sys_doeventsig(int eventID){
  struct event *my_event;

  mutex_lock(&event_lock);

  //check existence
  if ((event = get_event(eventID)) == NULL){
    mutex_unlock(&event_lock);
    printk("Invalid event ID\n");
    return -1;
  }

  // check privelege
  pUID = sys_geteuid();
  pGID = sys_getegid();

  if ((pUID != 0) 
      && (pUID != temp->UID) 
      && (my_event->user_enable == 1)){
    if ((pGID == temp->GID)
	&& (my_event->group_enable == 1)){
      mutex_unlock(&event_lock);
      printk("NO PRIVILEGE\n");
      return -1;
    }
  }

  my_event->sig = 1;
  wake_up_all(temp->event_queue);

  mutex_unlock(&event_lock);
}

// Fills in the array of integers pointed to by eventIDs 
// with the current set of active event IDs. num is the 
// number of integers which the memory pointed to by 
// eventIDs can hold. eventIDs can be NULL, in which case, 
// doeventinfo() returns the number of active event IDs. On 
// success, doeventinfo() returns the number of active 
// events; otherwise, it returns -1 on failure. If num is 
// smaller than the number of active event IDs, then -1 
// should be returned
//
// TODO - verify that we return event_count when eventIDs != NULL
asmlinkage long sys_doeventinfo(int num, int *eventIDs){
  struct list_head pos;
  long i;
  struct event *my_event;
  i = 0;
  if (eventIDs == NULL)
    return event_count;
  if (num < eventIDs)
    return -1;
  else{
    mutex_lock(&event_lock);
    list_for_each(pos, event_list_head){
      my_event = list_entry(pos, struct event, event_list);
      eventIDs[i++] = my_event->EID;
    }
    i = event_count;
    mutex_unlock(&event_lock);
  }
  return i;
}

// Change the UID and GID of the event to the specified 
// values; returns -1 on failure
asmlinkage long sys_doeventchown(int eventId, uid_t UID, gid_t GID){
  struct event *my_event;

  mutex_lock(&event_lock);

  //check existence
  if ((event = get_event(eventID)) == NULL){
    mutex_unlock(&event_lock);
    printk("Invalid event ID\n");
    return -1;
  }

  // check privelege
  pUID = sys_geteuid();
  pGID = sys_getegid();

  if ((pUID != 0) && (pUID != temp->UID)){
    mutex_unlock(&event_lock);
    printk("NO PRIVILEGE\n");
    preturn -2;
  }

  my_event->UID = UID;
  my_event->GID = GID;

  mutex_unlock(&event_lock);
}
// Place the UID, GID, User Signal Enable Bit, and Group 
// Signal Enable Bit into the memory pointed to by UID, GID, 
// UIDFlag, and GIDFlag, respectively; returns -1 on failure
asmlinkage long sys_doeventstat(int eventID, uid_t *UID, gid_t *GID, int *UIDFlag,
    int *GIDFlag){
  struct event *my_event = get_event(eventID);
  if (my_event == NULL)
    return -1;
  *UID = my_event->UID;
  *GID = my_event->GID;
  *UIDFlag = my_event->user_enable;
  *GIDFlag = my_event->group_enable;
  return 0;
}
