/*
 * event.h
 *
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <linux/types.h>
#include <linux/wait.h>
#include <linux/list.h>

struct event
{
	int EID; // event ID
	uid_t UID; // effective user ID
	gid_t GID; // effective group ID
	int user_enable; // user enable bit
	int group_enable; // group enable bit
	int event_sig; // event signal bit
	wait_queue_head_t event_queue; // waiting queue of the event
	struct list_head event_list; // list head of the event
};

void __init doevent_init(void);

#endif /* EVENT_H_ */
