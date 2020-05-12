#pragma once

#include "project_config.h"

// включить логирование
#define UMBA_TEST_LOGGING_ENABLED        1

// выключить подвисание на упавшем тесте
#ifndef USE_DOCKER
    #define UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED      1
    #define UMBA_TEST_DISABLE_IRQ()                    __disable_irq()
    #define UMBA_TEST_STOP_DEBUGGER()                  __BKPT(0xAA)
#else
    #define UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED      0
    #define UMBA_TEST_DISABLE_IRQ()
    #define UMBA_TEST_STOP_DEBUGGER()
#endif

namespace umba
{
    // максимальное количество тестов в одной группе
    const uint32_t tests_in_group_max = 200;

    // максимальное количество групп тестов
    const uint32_t groups_max = 100;
    
    // простой коллбэк в стиле С
    typedef void (*TaskCycleCallback)(void);
    
    const char COLOR_RESET[] = "\x1b[0m";
    const char RED_FG[]     = "\x1b[31m";
    const char GREEN_FG[]   = "\x1b[32m";
    const char RED_BG[]     = "\x1b[41m";
}