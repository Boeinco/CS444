// sstf-iosched.c
// group: Austin Nguyen, Isaac Stallcup, Alex Garcia

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
	sector_t end;
	struct list_head *other;
	int count;

};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	if (!list_empty(&nd->queue))
	{
	   struct request *req;
	   req = list_entry(nd->other, struct request, queuelist);
	   if (nd->count == 1)
	   {
	      list_del_init(&rq->queuelist);
	      nd->count = nd->count - 1;
	   }
	   else
	   {
	        struct request* curr_req = list_entry(nd->other, struct request, queuelist);
			struct request* prev_req = list_entry(nd->other->prev, struct request, queuelist);
			struct request* next_req = list_entry(nd->other->next, struct request, queuelist);

			long long curr = (long long)blk_rq_pos(curr_req);
			long long prev = (long long)blk_rq_pos(prev_req);
			long long next = (long long)blk_rq_pos(next_req);

			long long dprev, dnext = 0;
			dprev = abs(curr - prev);
			dnext = abs(curr - next);

			if (dprev < dnext) {
				nd->other = nd->other->prev;
			}
			else {
				nd->other = nd->other->next;
			}
			list_del_init(&rq->other);
			nd->count = nd->count - 1;
	   }
	   elv_dispatch_sort(q, rq);
	   return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	int x;
	if (list_empty(&nd->queue))
	{
		list_add(&rq->queuelist, &nd->queue);
		nd->other = nd->queue.next;
		nd->count++;
		return;
	}
	struct list_head* h;
	list_for_each(h, &nd->queue)
	{ 
		
		struct request* curr_req = list_entry(h, struct request, queuelist);
		struct request* curr_req_next = list_entry(h->next, struct request, queuelist);
		
		sector_t currsector = blk_rq_pos(curr_req);
		sector_t nextsector = blk_rq_pos(curr_req_next);
		sector_t newsector = blk_rq_pos(rq);
	
		if (nd->count == 1)
		{
			list_add(&rq->queuelist, h);
			nd->count++;
			x = 1;
			break;
		}	

		if (nextsector >= newsector && currsector <=newsector )
		{
			list_add(&rq->queuelist, position);
			nd->count++;
			x = 1;
			break;
		}
	}
	if (x != 1)
	{
		list_add_tail(&rq->queuelist, &nd->queue);
		nd->count++;
	}
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_prev_entry(rq, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_next_entry(rq, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;


	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		return NULL;
	}

	INIT_LIST_HEAD(&nd->queue);
	nd->count = 0;
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops.sq = {
	   	.elevator_allow_merge_fn 	= sstf_deny_merge,
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int sstf_deny_merge(struct request_queue *var, struct request *req, struct bio *b)
{
   return ELEVATOR_NO_MERGE;
}

static int __init sstf_init(void)
{
	int elv_register(&elevator_sstf);
	return 0;
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Group 12-08 CS444 Spring 2017");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sstf I/O scheduling implementation");
