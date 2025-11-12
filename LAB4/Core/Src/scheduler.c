/*
 * scheduler.c
 *
 *  Created on: Nov 12, 2025
 *      Author: ADMIN
 */


#include "scheduler.h"
#include "main.h"  // Nếu cần HAL_TIM, HAL_IWDG từ project STM32

// Global variables
sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t Error_code_G = 0;
static uint32_t last_error_code = 0;  // Cho report status
static uint32_t error_tick_count = 0;
static uint32_t counter_for_watchdog = 0;

// Tùy chọn: IWDG handle nếu dùng watchdog
IWDG_HandleTypeDef hiwdg;

void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Delete_Task(i);
    }
    Error_code_G = 0;  // Reset error
    // Init timer (ví dụ Timer 2 cho tick 10ms, tham khảo lab 3)
    // Timer_init();  // Tùy chỉnh code init timer
    Watchdog_Init();   // Nếu dùng
}

void SCH_Update(void) {
    uint8_t index;
    for (index = 0; index < SCH_MAX_TASKS; index++) {
        if (SCH_tasks_G[index].pTask) {
            if (SCH_tasks_G[index].Delay == 0) {
                SCH_tasks_G[index].RunMe++;
                if (SCH_tasks_G[index].Period) {
                    SCH_tasks_G[index].Delay = SCH_tasks_G[index].Period;
                }
            } else {
                SCH_tasks_G[index].Delay--;
            }
        }
    }
    Watchdog_Refresh();  // Nếu dùng
    // SCH_Report_Status();  // Có thể gọi ở đây hoặc dispatch
}

uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    uint32_t index = 0;
    while ((SCH_tasks_G[index].pTask != 0) && (index < SCH_MAX_TASKS)) {
        index++;
    }
    if (index == SCH_MAX_TASKS) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return SCH_MAX_TASKS;  // Error
    }
    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Delay = DELAY;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].RunMe = 0;
    SCH_tasks_G[index].TaskID = index;  // Gán ID
    return index;  // Trả về ID
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID >= SCH_MAX_TASKS || SCH_tasks_G[taskID].pTask == 0) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return 1;  // Error
    }
    SCH_tasks_G[taskID].pTask = 0;
    SCH_tasks_G[taskID].Delay = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].RunMe = 0;
    SCH_tasks_G[taskID].TaskID = NO_TASK_ID;
    return 0;  // Success
}

void SCH_Dispatch_Tasks(void) {
    uint8_t index;
    for (index = 0; index < SCH_MAX_TASKS; index++) {
        if (SCH_tasks_G[index].RunMe > 0) {
            (*SCH_tasks_G[index].pTask)();  // Chạy task
            SCH_tasks_G[index].RunMe--;
            if (SCH_tasks_G[index].Period == 0) {
                SCH_Delete_Task(index);  // One-shot
            }
        }
    }
    SCH_Report_Status();  // Tùy chọn
    SCH_Go_To_Sleep();   // Tùy chọn, vào idle mode
}

// Tùy chọn: Report status (Program 1.15)
void SCH_Report_Status(void) {
    // Implement hiển thị error qua LED hoặc port
    if (Error_code_G != last_error_code) {
        // Update port or LED
        last_error_code = Error_code_G;
        error_tick_count = 60000;  // 1 phút
    } else if (error_tick_count > 0) {
        error_tick_count--;
        if (error_tick_count == 0) Error_code_G = 0;
    }
}

// Tùy chọn: Go to sleep (Program 1.14)
void SCH_Go_To_Sleep(void) {
    // HAL_PWR_EnterSLEEPMode();  // Tùy MCU
}

// Tùy chọn: Watchdog functions (Program 1.17)
void Watchdog_Init(void) {
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
}

void Watchdog_Refresh(void) {
    HAL_IWDG_Refresh(&hiwdg);
}

uint8_t Is_Watchdog_Reset(void) {
    return (counter_for_watchdog > 3);
}

void Watchdog_Counting(void) {
    counter_for_watchdog++;
}

void Reset_Watchdog_Counting(void) {
    counter_for_watchdog = 0;
}
