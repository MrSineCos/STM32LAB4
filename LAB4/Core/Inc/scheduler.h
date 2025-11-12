/*
 * scheduler.h
 *
 *  Created on: Nov 12, 2025
 *      Author: ADMIN
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>  // Để dùng uint32_t, uint8_t

// Định nghĩa struct task (dựa trên gợi ý trong tài liệu)
typedef struct {
    void (*pTask)(void);  // Pointer đến function task
    uint32_t Delay;       // Initial delay (ticks)
    uint32_t Period;      // Interval (ticks), 0 nếu one-shot
    uint8_t RunMe;        // Flag để dispatch biết task sẵn sàng
    uint32_t TaskID;      // ID của task (hint từ tài liệu)
} sTask;

// Constants
#define SCH_MAX_TASKS 40     // Số task tối đa, điều chỉnh nếu cần
#define NO_TASK_ID 0         // ID không hợp lệ

// Error codes (tùy chọn, cho error reporting)
#define ERROR_SCH_TOO_MANY_TASKS 1
#define ERROR_SCH_CANNOT_DELETE_TASK 2
// ... thêm các error khác nếu cần

// Global variables (extern để declare)
extern sTask SCH_tasks_G[SCH_MAX_TASKS];
extern uint8_t Error_code_G;

// Function prototypes
void SCH_Init(void);
void SCH_Update(void);
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
void SCH_Dispatch_Tasks(void);

// Tùy chọn: Error reporting và power saving
void SCH_Report_Status(void);
void SCH_Go_To_Sleep(void);

// Tùy chọn: Watchdog (dựa trên Program 1.17)
void Watchdog_Init(void);
void Watchdog_Refresh(void);
uint8_t Is_Watchdog_Reset(void);
void Watchdog_Counting(void);
void Reset_Watchdog_Counting(void);

#endif // SCHEDULER_H
