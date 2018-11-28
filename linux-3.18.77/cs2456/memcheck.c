#include <linux/kernel.h>
#include <linux/event.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/list.h>

#define LRU_ALL_ACTIVE (BIT(LRU_ACTIVE_FILE) | BIT(LRU_ACTIVE_ANON))
#define LRU_ALL_INACTIVE (BIT(LRU_INACTIVE_FILE) | BIT(LRU_INACTIVE_ANON))

#define ONLY_ACTIVE 0
#define ONLY_INACTIVE 1
#define ACTIVE_REF_SET 2
#define INACTIVE_REF_SET 3
#define ACTIVE_INACTIVE 4
#define INACTIVE_BOOT 5

// GLOBAL VARIABLE //

// give it a pointer to an array you'd like to fill with stats
// 0 - success
// 1 - invalid pointer
int get_mem_stats(uint32_t *ret_stats){
  struct zone *zo;
  struct lru_vec *lvec;
  enum lru_list lru;
  struct list_head *hd;
  struct page *pg;

  uint32_t stats[6];

  for_each_zone(zo) {
    lvec = zo->zone_pgdat->lru_vec;
    for_each_lru(lru) {
      if ((BIT(lru) & LRU_ALL_ACTIVE)){
	// looking at actives
	list_for_each(hd, lvec[lru]){
	  ++stats[ACTIVE_ONLY];
	  // looking at a page
	  pg = list_entry(hd, struct page, lru);
	  if(PageReferenced(pg))
	    ++stats[ACTIVE_REF_SET];
	}
      } else if ((BIT(lru) & LRU_ALL_INACTIVE)){
	list_for_each(hd, lvec[lru]){
	  ++stats[INACTIVE_ONLY];
	  // looking at a page
	  pg = list_entry(hd, struct page, lru);
	  if(PageReferenced(pg))
	    ++stats[INACTIVE_REF_SET];
	}

      }
    }
  }
  unsigned long not_copied = copy_to_user(ret_stats, stats, sizeof(uint32_t)*6);
  if (not_copied)
    return 1;
  return 0;
}
