#ifndef SWAP_QUEUE_H
#define SWAP_QUEUE_H

#include <list>
using namespace std;

struct AOIData {
	int char_id;
	int x;
	int y;
};

//���߳�����aoi�̹߳�����Ҫ�õ��Ķ���
extern list<struct AOIData *> request_queue;

extern list<struct AOIData *> aoi_process_queue;

extern int m2a_swap_queue_lock;
extern list<struct AOIData *> m2a_swap_queue;


//aoi�߳̽�������������߳���Ҫ�õ��Ķ���
extern list<struct AOIData *> result_queue;

extern list<struct AOIData *> main_process_queue;

extern int a2m_swap_queue_lock;
extern list<struct AOIData *> a2m_swap_queue;

#endif //SWAP_QUEUE_H
