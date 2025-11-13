/*
 * scheduler.h
 *
 *  Created on: Nov 30, 2024
 *      Author: sine
 */
#include "main.h"

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include <stdint.h>
#include <stddef.h>

#define SCH_TASKNUMBER    8
#define SCH_TIMERTICK     10

typedef struct {
    void (*pTask)(void);
    uint8_t id;
    uint32_t delay;
    uint32_t period;
    unsigned char flag;
} SCH_Task;

SCH_Task tasks[SCH_TASKNUMBER];
void SCH_Init(void);
void SCH_Dispatch(void);

uint8_t SCH_AddTask(void (*pTask)(void), uint32_t delay, uint32_t period);

unsigned char SCH_DeleteTask(uint8_t id);

/* Ring buffer log mỗi lần dispatch để timePrint xả ra UART */
typedef struct {
    void (*pTask)(void);
    uint8_t id;
    uint32_t tick;
} SCH_DispatchLog;

#define SCH_LOG_SIZE 16
extern volatile SCH_DispatchLog SCH_DispatchLogs[SCH_LOG_SIZE];
extern volatile uint8_t SCH_LogHead;
extern volatile uint8_t SCH_LogTail;

#endif /* INC_SCHEDULER_H_ */
