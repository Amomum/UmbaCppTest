/*
 *  Пример конфигурационного файла
 */

#pragma once

#include "project_config.h"

// включить логирование
#define UMBA_TEST_LOGGING_ENABLED        1

#ifndef USE_DOCKER
    // Симулятор Кейла
    
    #define UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED      1
    #define UMBA_TEST_DISABLE_IRQ()                             __disable_irq()
    #define UMBA_TEST_STOP_DEBUGGER()                           __BKPT(0xAA)
    
    #define UMBA_TEST_ASSERT_WITH_EXCEPTION_ENABLED                  1
    #define UMBA_RUNTIME_STATIC_ASSERT_ENABLED                      1
    
#else
    // Docker с обычным gcc
    #include <assert.h>

    #define UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED      0
    #define UMBA_TEST_DISABLE_IRQ()
    #define UMBA_TEST_STOP_DEBUGGER()

    #define UMBA_TEST_USER_DEFINED_ASSERT( statement )           assert( statement )

    // заменяем ассерты на исключение
    #define UMBA_TEST_ASSERT_WITH_EXCEPTION_ENABLED                   1
    #define UMBA_RUNTIME_STATIC_ASSERT_ENABLED                       1
    
#endif

namespace umba
{
    // максимальное количество тестов в одной группе
    const uint32_t tests_in_group_max = 200;

    // максимальное количество групп тестов
    const uint32_t groups_max = 100;
    
    // простой коллбэк в стиле С - тип коллбэка, который будет дергаться при каждом обороте
    // бесконечного цикла в потоке ОС
    typedef void (*TaskCycleCallback)(void);
    

    const char COLOR_RESET[] = "\x1b[0m";
    const char RED_FG[]     = "\x1b[31m";
    const char GREEN_FG[]   = "\x1b[32m";
    const char RED_BG[]     = "\x1b[41m";
}
