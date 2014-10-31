/*
 * NOOP no more!(tm)
 * Exciting changes by Erin, Jafer, and John
 * currently:
 * sstf_data: added head_pos for disk head loc
 * sstf_dispatch: dispatch closest to disk head
 * sstf_add_request: add request to the queue, in order
 * sstf_balance: keep request queue sentinel close to disk head
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
	sector_t head_pos;
};


/**
 * sstf_balance represents an attempt at keeping the list ordered with the
 * list_head sentinel representing the position of the drive read head
 */
static void sstf_balance(struct sstf_data * sd) 
{
	struct request * rnext, * rprev;
	sector_t next, prev, pos;

	if(list_empty(&sd->queue)) 
		return;

	rnext = list_entry(sd->queue.next, struct request, queuelist);
	rprev = list_entry(sd->queue.prev, struct request, queuelist);
	
	next = rnext->sector;
	prev = rprev->sector;
	pos = sd->head_pos;

	while(1) {
		//right edge
		if(pos > prev && next < prev)
			break;
		//left edge
		if(pos < next && prev > next)
			break;
		//positioned middle
		if(pos < next && pos > prev)
			break;
		if(pos > next) {
			list_move(&sd->queue,&rnext->queuelist);
			rprev = rnext;
			prev = next;
			rnext = list_entry(sd->queue.next, struct request,
					   queuelist);
			next = rnext->sector;
		} else {
			list_move_tail(&sd->queue,&rprev->queuelist);
			rnext = rprev;
			next = prev;
			rprev = list_entry(sd->queue.prev, struct request,
					  queuelist);
			prev = rprev->sector;
		}
	}
}

static int get_distance(struct sstf_data * sd, struct request * rq)
{
	if(rq->sector < sd->head_pos) 
		return sd->head_pos - rq->sector;
	else 
		return rq->sector - sd->head_pos;
}

static void noop_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request * nextrq, * prevrq, * rq;  

		nextrq = list_entry(nd->queue.next, struct request, queuelist);
		prevrq = list_entry(nd->queue.prev, struct request, queuelist);

		if(get_distance(nd, nextrq) < get_distance(nd, prevrq)) 
			rq = nextrq;
		else
			rq = prevrq;

		list_del_init(&rq->queuelist);
		nd->head_pos = rq->sector + rq->nr_sectors - 1;
		elv_dispatch_sort(q, rq);
		//update queue head position
		sstf_balance(nd);
		if(rq_data_dir(rq) == 0)
			printk(KERN_INFO "[SSTF] dsp READ %ld\n",(long)rq->sector);
		else
			printk(KERN_INFO "[SSTF] dsp WRITE %ld\n",(long)rq->sector);
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *sd = q->elevator->elevator_data;

	struct request * rnext, * rprev;
	sector_t next, prev, pos;

	if(list_empty(&sd->queue))  {
		list_add(&rq->queuelist,&sd->queue);
		return;
	}

	rnext = list_entry(sd->queue.next, struct request, queuelist);
	rprev = list_entry(sd->queue.prev, struct request, queuelist);
	
	next = rnext->sector;
	prev = rprev->sector;
	pos = rq->sector;

	while(1) {
		//right edge
		if(pos > prev && next < prev)
			break;
		//left edge
		if(pos < next && prev > next)
			break;
		//positioned middle
		if(pos < next && pos > prev)
			break;
		if(pos > next) {
			rprev = rnext;
			prev = next;
			rnext = list_entry(sd->queue.next, struct request,
					   queuelist);
			next = rnext->sector;
		} else {
			rnext = rprev;
			next = prev;
			rprev = list_entry(sd->queue.prev, struct request,
					  queuelist);
			prev = rprev->sector;
		}
	}
	__list_add(&rq->queuelist, &rprev->queuelist, &rnext->queuelist);
	printk(KERN_INFO "[SSTF] add %ld",(long) rq->sector);
}

static int noop_queue_empty(struct request_queue *q)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	return list_empty(&nd->queue);
}

static struct request *
noop_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
noop_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static void *noop_init_queue(struct request_queue *q)
{
	struct sstf_data *nd;
	

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return NULL;
	INIT_LIST_HEAD(&nd->queue);
	nd->head_pos = 0;
	return nd;
}

static void noop_exit_queue(elevator_t *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_noop = {
	.ops = {
		.elevator_merge_req_fn		= noop_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_queue_empty_fn	= noop_queue_empty,
		.elevator_former_req_fn		= noop_former_request,
		.elevator_latter_req_fn		= noop_latter_request,
		.elevator_init_fn		= noop_init_queue,
		.elevator_exit_fn		= noop_exit_queue,
  },
	.elevator_name = "noop",
	.elevator_owner = THIS_MODULE,
};

static int __init noop_init(void)
{
	return elv_register(&elevator_noop);
}

static void __exit noop_exit(void)
{
	elv_unregister(&elevator_noop);
}

module_init(noop_init);
module_exit(noop_exit);


MODULE_AUTHOR("Erin, Jafer, John");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NOOP no more!(tm) IO scheduler ");

