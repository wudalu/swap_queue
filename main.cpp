#include "swap_queue.h"
#include "aoi_thread.h"

#include <pthread.h>
#include <list>

#include <iostream>
using namespace std;

extern pthread_t aoi_thread_tid;

//队列声明或定义
list<struct AOIData *> request_queue;

list<struct AOIData *> main_process_queue;

list<struct AOIData *> m2a_swap_queue;
extern list<struct AOIData *> a2m_swap_queue;

//锁声明或定义
int m2a_swap_queue_lock = 0;
extern int a2m_swap_queue_lock;

int fetch_results()
{
	while (__sync_lock_test_and_set(&a2m_swap_queue_lock, 1) != 0) {
	}

	int ret = 0;

	for (list<struct AOIData *>::iterator it=a2m_swap_queue.begin(); it!=a2m_swap_queue.end(); it++) {
		struct AOIData *temp = *it;
		main_process_queue.push_back(temp);

		++ret;
	}

	a2m_swap_queue.clear();

	__sync_lock_release(&a2m_swap_queue_lock);

	return ret;
}

void process_results()
{
	for (list<struct AOIData *>::iterator it=main_process_queue.begin(); it!=main_process_queue.end(); it++) {
		struct AOIData *temp = *it;
		printf("aoi done process, main showing results, char_id=%d, x=%d,y=%d\n", temp->char_id, temp->x, temp->y);
	}
}

void make_requests()
{
	for (int i=0; i<10; i++) {
		struct AOIData *data = new struct AOIData;
		data->char_id = i+100;
		data->x = 2 * i;
		data->y = 3 * i;
		
		//printf("make_requests,x=%d,y=%d\n", data->x, data->y);

		request_queue.push_back(data);
	}	
}

void do_something()
{
	make_requests();

	return ;
}

int push_request_to_swap()
{
	while (__sync_lock_test_and_set(&m2a_swap_queue_lock, 1) != 0) {
		//printf("main thread trying to get lock!\n");
	}

	//printf("main got queue, swap_queue_lock=%d\n", m2a_swap_queue_lock);

	int ret = 0;
	for (list<struct AOIData *>::iterator it=request_queue.begin(); it!=request_queue.end(); it++) {
		struct AOIData *temp = *it;
		m2a_swap_queue.push_back(temp);

		++ret;
	}

	request_queue.clear();

	__sync_lock_release(&m2a_swap_queue_lock);
	//printf("main release queue, swap_queue_lock=%d\n", m2a_swap_queue_lock);

	return ret;
}

void handle_results()
{
	int ret = fetch_results();
	if (ret <= 0) {
		printf("this round, aoi doing nothing!\n");
	}

	process_results();
}

int main(int argc, char **argv)
{
	bool ret = create_aoi_thread();
	if (!ret) {
		cout<<"error creating aoi_thread!"<<endl;
		return -1;
	}

	//main thread working
	while (true) {
		//做事情
		do_something();

		//最后，将request_queue的数据push到swap_queue
		int count = push_request_to_swap();
		if (count < 0) {
			printf("m2a push empty!!!");
			continue ;
		}

		handle_results();
	}

	return 0;
}

