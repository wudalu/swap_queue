#ifndef SWAP_QUEUE_H
#define SWAP_QUEUE_H

#include <list>
using namespace std;

struct AOIData {
	int char_id;
	int x;
	int y;
};

//主线程请求aoi线程工作需要用到的队列
extern list<struct AOIData *> request_queue;

extern list<struct AOIData *> aoi_process_queue;

extern int m2a_swap_queue_lock;
extern list<struct AOIData *> m2a_swap_queue;


//aoi线程将结果交换回主线程需要用到的队列
extern list<struct AOIData *> result_queue;

extern list<struct AOIData *> main_process_queue;

extern int a2m_swap_queue_lock;
extern list<struct AOIData *> a2m_swap_queue;

#endif //SWAP_QUEUE_H
