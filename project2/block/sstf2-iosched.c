/*
 * Team 04 Nicholas Nelson, Jonathan McNeil, Kui Wang
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

static void
sstf_merged_requests(struct request_queue * q, struct request * rq,
		     struct request * next)
{
	list_del_init(&next->queuelist);
}

static int
sstf_dispatch(struct request_queue * q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	//sector_t * curr_sector = (sector_t *) q->queuedata;	
	struct request *temp, *temp_next, *temp_prev, *rq, 
	  *rq_above, *rq_below;
	
	if (!list_empty(&nd->queue)) {
   	        // this is if there is only one element in the queue
	  
	        if (nd->queue.prev == nd->queue.next) {		
			rq = list_entry(nd->queue.next, 
					struct request, queuelist);
			//*curr_sector = rq->sector + rq->nr_sectors;
			nd->head_pos = rq->sector + rq->nr_sectors - 1;
			list_del_init(&rq->queuelist);

			if (rq->cmd_flags & REQ_RW)
				printk("[SSTF] dsp READ  %llu 1\n",
				       (unsigned long long)  rq->sector);
			else
				printk("[SSTF] dsp WRITE %llu 1\n",
				       (unsigned long long)  rq->sector);

			elv_dispatch_sort(q, rq);
		}
		
		
		else {
	  //    printk("line 47 else\n");
		        // multiple elements in queue
			rq_above = list_entry(nd->queue.next, 
					      struct request, queuelist);
			rq_below = list_entry(nd->queue.prev, 
					      struct request, queuelist);
			// below request is closer
	  //  	printk("line 52\n");
			if (nd->head_pos - rq_below->sector < 
			    rq_above->sector - nd->head_pos) {		
				/* ((sector_t *) q->queuedata) = 
				 * rq_below->sector + rq_below->nr_sectors;
				 */
				nd->head_pos = rq_above->sector + 
				  rq_above->nr_sectors - 1;
				list_del_init(&rq_below->queuelist);

				if (rq_below->cmd_flags & REQ_RW)
					printk("[SSTF] dsp READ  %llu 2\n",
					       (unsigned long long)  
					       rq_below->sector);
				else
					printk("[SSTF] dsp WRITE %llu 2\n",
					       (unsigned long long)  
					       rq_below->sector);

				elv_dispatch_sort(q, rq_below);
			}
			
			else {/* above request is closer
			       * ((sector_t *) q->queuedata) = 
			       * rq_above->sector + rq_above->nr_sectors;
			       */
	 //   	        printk("line 70\n");
				nd->head_pos = rq_above->sector + 
				               rq_above->nr_sectors - 1;
				list_del_init(&rq_above->queuelist);

				if (rq_above->cmd_flags & REQ_RW)
					printk("[SSTF] dsp READ  %llu 3\n",
					       (unsigned long long)  
					       rq_above->sector);
				else
					printk("[SSTF] dsp WRITE %llu 3\n",
					       (unsigned long long)  
					       rq_above->sector);

				elv_dispatch_sort(q, rq_above);
			}
		}
	//    printk("line 77\n");
		/*queuedata and head desync checking logic*/
		// if it has gone too high 
		if (nd->head_pos > list_entry(nd->queue.next, 
					      struct request, 
					      queuelist)->sector) {	
   // 	  printk("for loop each entry in Dispatch\n");
		        // cycle through the list looking for
			list_for_each_entry(temp, &nd->queue, queuelist) { 
   // 			printk("Inside of dsp/FOR\n");
				temp_next = list_entry(temp->queuelist.next, 
						       struct request, 
						       queuelist);
				temp_prev = list_entry(temp->queuelist.prev, 
						       struct request, 
						       queuelist);
				// a larger entry than the curr_sector
				if (&temp->sector != NULL && &temp->sector > 
				    &nd->head_pos) {	 
				  	 // and move it to right before
					list_move_tail(&(nd->queue), 
						       &(temp->queuelist));
					break;
				} 
				else if ( &temp->sector != NULL && 
					  &temp->sector < nd->head_pos && 
					  &temp_next->sector < &temp->sector) {
					list_move(&nd->queue, 
						  &temp->queuelist);
					break;
				}
				else if ( &temp->sector != NULL && 
					  &temp_next->sector > nd->head_pos && 
					  &temp_next->sector < &temp->sector) {
					list_move(&nd->queue, 
						  &temp->queuelist);
					break;
				}
			}
		}
	//	printk("line 119 dongs.\n");
		return 1;
	}
	return 0;
	/*
	// ORIGINAL IMPLEMENTATION (noop_dispatch)
	// saved for reference
	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
	*/
}

static void
sstf_add_request(struct request_queue * q, struct request * rq)
{

	struct sstf_data *nd = q->elevator->elevator_data;
	struct request *temp;
	struct request *next, *prev;
	next = list_entry(nd->queue.next, struct request, queuelist);
	prev = list_entry(nd->queue.prev, struct request, queuelist);
	if (list_empty(&nd->queue)) {

		if (rq->cmd_flags & REQ_RW)
			printk("[SSTF] add READ  %llu 4\n",
			       (unsigned long long)  rq->sector);
		else
			printk("[SSTF] add WRITE %llu 4\n",
			       (unsigned long long)  rq->sector);

		list_add_tail(&rq->queuelist, &nd->queue);
	}
	else if (next->sector > rq->sector
		 && rq->sector > prev->sector) {

		if (rq->sector > q->end_sector) {
			list_add(&rq->queuelist, &nd->queue);
			if (rq->cmd_flags & REQ_RW)
				printk("[SSTF] add READ  %llu 5\n",
				       (unsigned long long)  rq->sector);
			else
				printk("[SSTF] add WRITE %llu 5\n",
				       (unsigned long long)  rq->sector);
		}

		else {
			list_add_tail(&rq->queuelist, &nd->queue);

			if (rq->cmd_flags & REQ_RW)
				printk("[SSTF] add READ  %llu 6\n",
				       (unsigned long long)  rq->sector);
			else
				printk("[SSTF] add WRITE %llu 6\n",
				       (unsigned long long)  rq->sector);
		}
	}
	//Loops through the list, insertion sorting.
		else {
   // 	  printk("for list each entry thing Add\n");
		list_for_each_entry(temp, &nd->queue, queuelist) {
   // 		printk("Inside of for loop in ADD");
			if (&temp->sector != NULL && 
			    &temp->sector < &rq->sector) {
   //			printk("Inside of first if in for/ADD");
				next = list_entry(temp->queuelist.next, 
						  struct request, queuelist);
				if (&next->sector != NULL && 
				    &next->sector > &rq->sector) {
   // 				printk("Inside of second if in for/ADD");
					list_add(&rq->queuelist, 
						 &next->queuelist);
					if (rq->cmd_flags & REQ_RW)
						printk("[SSTF] add READ  %llu
                                                       \n",
						       (unsigned long long)  
						       rq->sector);
					else
						printk("[SSTF] add WRITE %llu
                                                        \n",
						       (unsigned long long)  
						       rq->sector);
				}
				else
					return;
			}
		}
	}
	//list_add_tail(&rq->queuelist, &nd->queue);
}

static int
sstf_queue_empty(struct request_queue * q)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	return list_empty(&nd->queue);
}

static struct request *
sstf_former_request(struct request_queue * q, struct request * rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue * q, struct request * rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static void    *
sstf_init_queue(struct request_queue * q)
{
	struct sstf_data *nd;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return NULL;
	INIT_LIST_HEAD(&nd->queue);
	nd->head_pos = 0;
	return nd;
}

static void
sstf_exit_queue(elevator_t * e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn = sstf_merged_requests,
		.elevator_dispatch_fn = sstf_dispatch,
		.elevator_add_req_fn = sstf_add_request,
		.elevator_queue_empty_fn = sstf_queue_empty,
		.elevator_former_req_fn = sstf_former_request,
		.elevator_latter_req_fn = sstf_latter_request,
		.elevator_init_fn = sstf_init_queue,
		.elevator_exit_fn = sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int      __init
sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void     __exit
sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("CS444 - Group 4");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");