#include "aoi_thread.h"
#include "swap_queue.h"

#include <stdio.h>

//队列声明或定义
list<struct AOIData *> result_queue;

list<struct AOIData *> aoi_process_queue;

extern list<struct AOIData*> m2a_swap_queue;
list<struct AOIData *> a2m_swap_queue;

//锁声明或定义
int a2m_swap_queue_lock = 0;
extern int m2a_swap_queue_lock;

int fetch_swap_to_process()
{
	while (__sync_lock_test_and_set(&m2a_swap_queue_lock, 1) != 0) {
		//printf("aoi_thread trying to get lock!\n");
	}

	//printf("aoi_thread got queue, swap_queue_lock=%d\n", m2a_swap_queue_lock);

	int ret = 0;
	for (list<struct AOIData *>::iterator it=m2a_swap_queue.begin(); it!=m2a_swap_queue.end(); it++) {
		struct AOIData *temp = *it;
		aoi_process_queue.push_back(temp);
		
		++ret;
	}

	m2a_swap_queue.clear();

	__sync_lock_release(&m2a_swap_queue_lock);
	//printf("aoi_thread release queue, swap_queue_lock=%d\n", m2a_swap_queue_lock);

	return ret;
}

void process_data_queue()
{
	for (list<struct AOIData *>::iterator it=aoi_process_queue.begin(); it!=aoi_process_queue.end(); ++it) {
		struct AOIData *temp = *it;
		temp->x = 100;
		temp->y = 200;

		//将处理过后的结果放入到结果队列中
		result_queue.push_back(temp);
	}

	aoi_process_queue.clear();

	return ;
}

int push_result_to_swap()
{
	while (__sync_lock_test_and_set(&a2m_swap_queue_lock, 1) != 0) {
	}

	int ret = 0;
	for (list<struct AOIData *>::iterator it=result_queue.begin(); it!= result_queue.end(); it++) {
		struct AOIData *temp = *it;
		a2m_swap_queue.push_back(temp);

		++ret;
	}

	result_queue.clear();

	__sync_lock_release(&a2m_swap_queue_lock);

	return ret;
}

void * handle_aoi(void *arg)
{
	while (true) {
		//将m2a_swap_queue里面的数据取到aoi_process_queue
		int count = fetch_swap_to_process();			
		if (count <= 0)
			continue;

		//处理process_queue中的数据
		process_data_queue();

		//将result_queue的数据push到a2m_swap_queue
		int ret = push_result_to_swap();
		if (ret < 0) {
			printf("a2m push empty!!!");
			continue ;
		}
	}

	return NULL;
}

pthread_t create_aoi_thread()
{
	pthread_t tid;
	int err = pthread_create(&tid, NULL, handle_aoi, NULL);
	if (err != 0) {
		printf("can't create thread: %s\n", strerror(err));
		return 0;
	}

	return tid;
}
