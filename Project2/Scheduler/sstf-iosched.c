#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
	sector_t last_sector; 
	struct list_head* enqueue;
	
	int queue_count;
};

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->enqueue, struct request, queuelist);

		if (rq == 0) {
			printk("failed to add request to queue\n");
			return 0;
		}

		if (nd->queue_count == 1) {
			list_del_init(&rq->queuelist);
			nd->queue_count--;
		}

		else {
			//Get the pointers to the requests that we want
			struct request* curr_req = list_entry(nd->enqueue, struct request, queuelist);
			struct request* prev_req = list_entry(nd->enqueue->prev, struct request, queuelist);
			struct request* next_req = list_entry(nd->enqueue->next, struct request, queuelist);
		
			//Get the sectors	
			unsigned long curr = (unsigned long)blk_rq_pos(curr_req);
			unsigned long prev = (unsigned long)blk_rq_pos(prev_req);
			unsigned long next = (unsigned long)blk_rq_pos(next_req);
		
			//Get the differences	
			unsigned long diff_prev = 0;
			unsigned long diff_next = 0;

			diff_prev = abs(curr - prev);
			diff_next = abs(curr - next);

			//If prev is closer to current, then move enqueue to prev and dispatch
			if (diff_prev < diff_next) {
				nd->enqueue = nd->enqueue->prev;
			}
			//else move enqueue to next and dispatch
			else {
				nd->enqueue = nd->enqueue->next;
			}	
			
			//Delete currently dispatched request because it's finished
        	list_del_init(&rq->queuelist);
			nd->queue_count--;
		}

        printk("dispatching request: %lu\n", (unsigned long)blk_rq_pos(rq));
		elv_dispatch_sort(q, rq);
        return 1;
    }
    return 0;
}

static void sstf_print_list(struct request_queue *q)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	struct list_head* position_print;
	struct request* print_node;
	printk("print list: ");
	list_for_each(position_print, &nd->queue) {
		print_node = list_entry(position_print, struct request, queuelist);
		printk("%lu,", (unsigned long)blk_rq_pos(print_node));
	}
	printk("\n");
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	
	struct sstf_data *nd = q->elevator->elevator_data;
	int added = 0;
	
	printk("adding request: %lu\n", (unsigned long)blk_rq_pos(rq));

	//sstf_print_list(q);

	//If the list is empty, do a basic add and return
	if (list_empty(&nd->queue)){
		list_add(&rq->queuelist, &nd->queue);
		nd->enqueue = nd->queue.next;  
		nd->queue_count++;
        printk ("list is empty\n");
		//sstf_print_list(q);
		return;
	}

	struct list_head* head;
	list_for_each(head, &nd->queue) { 
		
		struct request* curr_req = list_entry(head, struct request, queuelist);
		struct request* curr_req_next = list_entry(head->next, struct request, queuelist);
		
		sector_t curr_req_sector = blk_rq_pos(curr_req);
		sector_t next_req_sector = blk_rq_pos(curr_req_next);
		sector_t new_req_sector = blk_rq_pos(rq);
	
		printk ("curr= %lu and next= %lu\n" , (unsigned long)blk_rq_pos(curr_req), (unsigned long)blk_rq_pos(curr_req_next));

		//This is for one item in the queue	
		if (nd->queue_count == 1){
			list_add(&rq->queuelist, head);
			nd->queue_count++;
			added = 1;
			break;
		}	

		//This is for general queue addition	
		if (next_req_sector >= new_req_sector && curr_req_sector <=new_req_sector ){
			list_add(&rq->queuelist, position);
			nd->queue_count++;
			added = 1;
			break;
		}
    }

	if(added != 1) {
		// Our new request must be bigger than all current, add it to the end
		list_add_tail(&rq->queuelist, &nd->queue);
		nd->queue_count++;
		printk("added to tail\n");
	}

	printk("adding complete\n");
	printk("queue count: %d\n", nd->queue_count);
	//sstf_print_list(q);
}

static void *sstf_init_queue(struct request_queue *q)
{
	struct sstf_data *nd;
	printk("queue initialized\n");
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return NULL;
	INIT_LIST_HEAD(&nd->queue);
	nd->queue_count = 0;
	return nd;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static int sstf_deny_merge(struct request_queue *req_q, struct request *req,
			struct bio *bio)
{
	return ELEVATOR_NO_MERGE;
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_allow_merge_fn 	= sstf_deny_merge,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_init_fn			= sstf_init_queue,
		.elevator_exit_fn			= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	elv_register(&elevator_sstf);

	return 0;
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("CS444 - Group 4");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
