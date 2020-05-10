#pragma once

#include "project_config.h"

// включить логирование
#define UMBA_TEST_LOGGING_ENABLED        1

// выключить подвисание на упавшем тесте
#define UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED      1

#define UMBA_TEST_DISABLE_IRQ()  __disable_irq()


namespace umba
{
    // максимальное количество тестов в одной группе
    const uint32_t tests_in_group_max = 200;

    // максимальное количество групп тестов
    const uint32_t groups_max = 100;
    
    // простой коллбэк в стиле С
    typedef void (*TaskCycleCallback)(void);
    
    // делегат
    //typedef util::Callback<void (void)> TaskCycleCallback;
    
}
