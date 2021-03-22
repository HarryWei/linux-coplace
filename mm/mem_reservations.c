/*
 * mm/mem_reservations.c - mechanism for reserving 32KB chunks of physical memory 
 *                         to accelerate page walks when running under virtualization
 *
 */

#include <linux/mm.h>
#include <linux/slab.h> 
#include <linux/highmem.h>
#include <linux/vmstat.h>
#include <linux/mem_reservations.h>

extern int ca_pid;

struct rm_node* rm_node_create() {
  struct rm_node* new = NULL;
  unsigned int i;
  new = kmalloc(sizeof(struct rm_node), GFP_KERNEL & ~__GFP_DIRECT_RECLAIM);
  if (new) {
    for (i = 0; i < RT_NODE_RANGE_SIZE; i++) {
      spin_lock_init(&new->items[i].lock);
      new->items[i].next_node = NULL;
    }
  }
  return new;
}

extern void rm_release_reservation(struct vm_area_struct *vma, unsigned long address) {
  unsigned char level;
  unsigned int i;
  unsigned int index;
  int unused;

  struct rm_node *cur_node = GET_RM_ROOT(vma);
  struct rm_node *next_node;
  
  unsigned long leaf_value;
  unsigned char mask;

  struct page *page;
  spinlock_t  *next_lock;

  gfp_t gfp           = ((GFP_HIGHUSER | __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM);
	unsigned long haddr = address & RESERV_MASK; 
  int region_offset   = (address & (~RESERV_MASK)) >> PAGE_SHIFT;
  //bool my_app         = (vma->vm_mm->owner->pid == 5555);
  bool my_app         = (vma->vm_mm->owner->pid == ca_pid);

  if (!my_app) 
    return;
  if (cur_node == NULL) 
    return;
  if (!vma_is_anonymous(vma)) {
    return;
  }

  // traverse the reservation map radix tree
  // firstly, go through all levels but don't go to the leaf node
  for (level = 1; level < NUM_RT_LEVELS; level++) {
    index = get_node_index(level, address);
    next_lock = &cur_node->items[index].lock;
    next_node = cur_node->items[index].next_node;

    if (unlikely(next_node == NULL)) {
      spin_lock(next_lock);
      if (next_node == NULL) {
        cur_node->items[index].next_node = rm_node_create();
      }
      spin_unlock(next_lock);
    }

    cur_node = cur_node->items[index].next_node;
  }

  // secondly, process the leaf node
  level = NUM_RT_LEVELS;
  index = get_node_index(level, address); 
  next_lock = &cur_node->items[index].lock;

  spin_lock(next_lock);
  leaf_value = (unsigned long)(cur_node->items[index].next_node);
  if (leaf_value != 0) { 
    page = get_page_from_rm(leaf_value);
    mask = get_mask_from_rm(leaf_value);

    unused = 8;
    while (mask) {
      unused -= mask & 1;
      mask = (mask >> 1);
    }
    if (unused) {
      mod_node_page_state(page_pgdat(page), NR_MEM_RESERVATIONS_RESERVED, -unused);
    }

    for (i = 0; i < RESERV_NR; i++) {
      put_page(page + i);
    }

    cur_node->items[index].next_node = 0; 
  }
  spin_unlock(next_lock);
  return;
}

bool check_from_reservation(struct vm_area_struct *vma, unsigned long address) {
  unsigned char level;
  unsigned int i;
  unsigned int index;

  struct rm_node *cur_node = GET_RM_ROOT(vma);
  struct rm_node *next_node;
  
  unsigned long leaf_value;
  unsigned char mask;

  struct page *page;
  spinlock_t  *next_lock;

  gfp_t gfp           = ((GFP_HIGHUSER | __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM);
	unsigned long haddr = address & RESERV_MASK; 
  int region_offset   = (address & (~RESERV_MASK)) >> PAGE_SHIFT;
  //bool my_app         = (vma->vm_mm->owner->pid == 5555);
  bool my_app         = (vma->vm_mm->owner->pid == ca_pid);

  if (!my_app) 
    return false;
  if (cur_node == NULL) 
    return false;
  if (!vma_is_anonymous(vma)) {
    return false;
  }

  // traverse the reservation map radix tree
  // firstly, go through all levels but don't go to the leaf node
  for (level = 1; level < NUM_RT_LEVELS; level++) {
    index = get_node_index(level, address);
    next_lock = &cur_node->items[index].lock;
    next_node = cur_node->items[index].next_node;

    if (unlikely(next_node == NULL)) {
      spin_lock(next_lock);
      if (next_node == NULL) {
        cur_node->items[index].next_node = rm_node_create();
      }
      spin_unlock(next_lock);
    }

    cur_node = cur_node->items[index].next_node;
  }

  // secondly, process the leaf node
  level = NUM_RT_LEVELS;
  index = get_node_index(level, address); 
  next_lock = &cur_node->items[index].lock;

  spin_lock(next_lock);
  leaf_value = (unsigned long)(cur_node->items[index].next_node);
  spin_unlock(next_lock);
  if (leaf_value != 0) { 
    return true;
  } else {
    return false;
  }
}

struct page *rm_alloc_from_reservation(struct vm_area_struct *vma, unsigned long address) {
  unsigned char level;
  unsigned int i;
  unsigned int index;

  struct rm_node *cur_node = GET_RM_ROOT(vma);
  struct rm_node *next_node;
  
  unsigned long leaf_value;
  unsigned char mask;

  struct page *page;
  spinlock_t  *next_lock;

  gfp_t gfp           = ((GFP_HIGHUSER | __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM);
	unsigned long haddr = address & RESERV_MASK; 
  int region_offset   = (address & (~RESERV_MASK)) >> PAGE_SHIFT;
  //bool my_app         = (vma->vm_mm->owner->pid == 5555);
  bool my_app         = (vma->vm_mm->owner->pid == ca_pid);

  if (!my_app) 
    return NULL;
  if (cur_node == NULL) 
    return false;
  if (!vma_is_anonymous(vma)) {
    return NULL;
  }

  // traverse the reservation map radix tree
  // firstly, go through all levels but don't go to the leaf node
  for (level = 1; level < NUM_RT_LEVELS; level++) {
    index = get_node_index(level, address);
    next_lock = &cur_node->items[index].lock;
    next_node = cur_node->items[index].next_node;

    if (unlikely(next_node == NULL)) {
      spin_lock(next_lock);
      if (next_node == NULL) {
        cur_node->items[index].next_node = rm_node_create();
      }
      spin_unlock(next_lock);
    }

    cur_node = cur_node->items[index].next_node;
  }

  // secondly, process the leaf node
  level = NUM_RT_LEVELS;
  index = get_node_index(level, address); 
  next_lock = &cur_node->items[index].lock;

  spin_lock(next_lock);
  leaf_value = (unsigned long)(cur_node->items[index].next_node);
  page = get_page_from_rm(leaf_value);
  if (leaf_value == 0) { //create a new reservation if not present 
    // allocate pages 
    //page = alloc_pages_vma(gfp, RESERV_ORDER, vma, haddr, numa_node_id(), false); 
    page = alloc_pages_vma(gfp, RESERV_ORDER, vma, haddr, numa_node_id()); 
    for (i = 0; i < RESERV_NR; i++) {
      set_page_count(page + i, 1);
    }
    // create a leaf node
    leaf_value = create_value(page, 0);
    mod_node_page_state(page_pgdat(page), NR_MEM_RESERVATIONS_RESERVED, RESERV_NR - 1);
    count_vm_event(MEM_RESERVATIONS_ALLOC);
  } else {
    mod_node_page_state(page_pgdat(page), NR_MEM_RESERVATIONS_RESERVED, -1);
  }
  page = page + region_offset;

  // mark the page as used
  mask = get_mask_from_rm(leaf_value);
  SET_BIT(mask, region_offset); 
  leaf_value = update_mask(leaf_value, mask);
  cur_node->items[index].next_node = (void*)(leaf_value);

  get_page(page);
  clear_user_highpage(page, address);

  spin_unlock(next_lock);
  count_vm_event(MEM_RESERVATIONS_RECEIVED_BY_PID_5555);

  return page;
}

void rm_destroy(struct rm_node *node, unsigned char level) { //not thread-safe 
  unsigned int index;
  int i;
  struct rm_node *cur_node = node;
  unsigned char mask;
  unsigned char unused;
  struct page *page;
  unsigned long leaf_value;

  // traverse the reservaton map radix tree
  for (index = 0; index < RT_NODE_RANGE_SIZE; index++) {
    if (cur_node->items[index].next_node != NULL) {
      if (level != 4) {
        rm_destroy(cur_node->items[index].next_node, level + 1);
      } else {
        leaf_value = (unsigned long)(cur_node->items[index].next_node);
        page = get_page_from_rm(leaf_value);
        mask = get_mask_from_rm(leaf_value);

        unused = 8;
        while (mask) {
          unused -= mask & 1;
          mask = (mask >> 1);
        }
        if (unused) {
          mod_node_page_state(page_pgdat(page), NR_MEM_RESERVATIONS_RESERVED, -unused);
        }

        for (i = 0; i < RESERV_NR; i++) {
          put_page(page + i);
        }
      }
    }
  }
  kfree(cur_node);
  return;
}
