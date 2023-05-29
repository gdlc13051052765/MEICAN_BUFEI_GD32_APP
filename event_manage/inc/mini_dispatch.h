#ifndef  __MINI_DISPATCH_H
#define  __MINI_DISPATCH_H
#include <stdint.h>

typedef void (*task_hook_func)(void*);

typedef enum 
{
	TASK_ENABLE=1,
	TASK_DISABLE
}TASK_STA;

// 任务结构
typedef struct _TASK_COMPONENTS
{
	uint8_t 		Run;                 		//程序运行标记：0-不运行，1运行
	TASK_STA		RunState;								//运行状态，TASK_ENABLE：使能检测    TASK_DISABLE：不进行检测
	//		uint16_t    Priority;								//优先级
	uint32_t  	run_power;						//计是个数，每记满一次加一
	uint32_t 		Timer;              		//计时器
	uint32_t 		ItvTime;              	//任务运行间隔时间，下一次填充时间
	void (*TaskHook)(void*);    		// 要运行的任务函数
} TASK_COMPONENTS;       					// 任务定义


// 任务清单
typedef enum _TASK_LIST
{   
	READ_CARD_TASK,						//读写卡任务
	PT113_REC_TASK,						//解析主控T113发过来的数据任务
	P485_SEN_TASK,						//485发送任务
	P485_REC_TASK,						//485接收任务
	TEST_TASK,								//测试任务
	TASKS_MAX                 //总的可供分配的定时任务数目
} TASK_LIST;

void TaskSetTimes(TASK_LIST Task , uint32_t Times);
void TaskRefresh(TASK_LIST Task);
uint8_t IsTaskEnable(TASK_LIST Task);
void EnableTask(TASK_LIST Task);
void DisableTask(TASK_LIST Task);
void TaskRemarks(void);
void TaskProcess(void*);
#endif
