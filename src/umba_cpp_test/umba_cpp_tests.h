/***************************************************************************************************
  Это основной заголовочный файл "фреймворка" UMBA_CPP_TESTS.
  
  Можете даже попробовать его понять. 
  
  Использование:

      - создайте группу тестов с помощью макроса UMBA_TEST_GROUP( "group name" )

      - создайте функции setup и teardown с помощью макросов UMBA_TEST_SETUP и UMBA_TEST_TEARDOWN.
        Setup будет запускаться перед каждым тестом, teardown - после каждого теста.

      - создайте тест с помощью макроса UMBA_TEST("test name"); тест должен возвращать 0

      - для проверок внутри теста используйте макрос UMBA_CHECK( condition, "description" )

      - для тестирования потоков ОС используйте функции setTaskCycleCounter, decrementTaskCycleCounter,
        setTaskCycleCallback. Цикл внутри потока должен быть while(OS_IS_RUNNING)


  Настройки:

      - должны лежать в файле umba_cpp_test_config.h в namespace umba

      - const uint32_t tests_in_group_max - максимальное количество тестов в одной группе

      - const uint32_t groups_max - максимальное количество групп тестов

      - переопределение типа TaskCycleCallback; это должно быть что-то, вызываемое через () без параметров

      опционально:

      - дефайн UMBA_TEST_LOGGING_ENABLED 1 включает логирование (т.е. каждый тест рапортует о запуске).
        По-умолчанию выключено.

      - дефайн UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED 0 выключает подвисание прямо на проваленном тесте.
        По-умолчанию включено.
        
      - дефайн UMBA_TEST_FPUTC_REDEFINITION_ENABLED 1 разрешает переопределять fputc через ITM_Sendchar
        По-умолчанию включено


***************************************************************************************************/

#pragma once

#include <stdio.h>
#include <stdint.h>

#include "umba_cpp_test_config.h"

namespace umba
{

    typedef const char * (*TestFunction)(const char * umbaTestName);
    
    typedef int (*GroupRunner)(void);
    
    void runAllTests();
    
    void addTestGroupRunner(GroupRunner groupRunner);

    void incrementTestsRun(void);

    uint32_t getTestsRun(void);

    // для тестирования задач OS

    // счетчик требуемого количества проходов основного цикла задачи при тестировании
    void setTaskCycleCounter(uint32_t cnt);

    uint32_t decrementTaskCycleCounter(void);

    // этот коллбек вызывается внутри decrementTaskCycleCounter
    // т.е. на каждом обороте цикла
    void setTaskCycleCallback( TaskCycleCallback cb );



} // namespace umba

#define UMBA_TEST_SETUP()   static void umba :: setup(void)

#define UMBA_TEST_TEARDOWN() static void umba :: teardown(void)


#ifndef UMBA_TEST_LOGGING_ENABLED
    #define UMBA_TEST_LOGGING_ENABLED  0
#endif

#if UMBA_TEST_LOGGING_ENABLED == 1

#define UMBA_LOCAL_GROUP_RUNNER                                                               \
                                                                                              \
static int runLocalGroup(void)                                                                \
{                                                                                             \
    int groupResult = 0;                                                                      \
    for(uint32_t i=0; i<addedTests; i++)                                                      \
    {                                                                                         \
                                                                                              \
        ::umba::incrementTestsRun();                                                          \
        setup();                                                                              \
                                                                                              \
        printf("Starting test -- %s -- in group \"%s\"", testNames[i], groupName);            \
                                                                                              \
        const char * result = tests[i]( testNames[i] );                                       \
                                                                                              \
        teardown();                                                                           \
                                                                                              \
        /* какой-то тест провалился - выводим сообщение об ошибке */                          \
        /* в режиме с остановкой - тест сам выведет сообщение*/                               \
        if( result != 0)                                                                      \
        {                                                                                     \
            printf("\n\nIn group \"%s\" failed test \"%s\":\n  ", groupName, testNames[i]);   \
            fputs(result, stdout);                                                            \
            printf("\n\n");                                                                   \
            groupResult = 1;                                                                  \
        }                                                                                     \
        else                                                                                  \
        {                                                                                     \
            printf("...OK\n");                                                                \
        }                                                                                     \
                                                                                              \
    }                                                                                         \
                                                                                              \
    return groupResult;                                                                       \
}

#else

#define UMBA_LOCAL_GROUP_RUNNER                                                               \
                                                                                              \
static int runLocalGroup(void)                                                                \
{                                                                                             \
    int groupResult = 0;                                                                      \
    for(uint32_t i=0; i<addedTests; i++)                                                      \
    {                                                                                         \
                                                                                              \
        ::umba::incrementTestsRun();                                                          \
        setup();                                                                              \
                                                                                              \
        const char * result = tests[i]( testNames[i] );                                       \
                                                                                              \
        teardown();                                                                           \
                                                                                              \
        /* какой-то тест провалился - выводим сообщение об ошибке */                          \
        /* в режиме с остановкой - тест сам выведет сообщение*/                               \
        if( result != 0)                                                                      \
        {                                                                                     \
            printf("\nIn group \"%s\" failed test \"%s\":\n  ", groupName, testNames[i]);     \
            fputs(result, stdout);                                                            \
            printf("\n\n");                                                                   \
            groupResult = 1;                                                                  \
        }                                                                                     \
    }                                                                                         \
                                                                                              \
    return groupResult;                                                                       \
}

#endif


#define UMBA_TEST_GROUP( group_name )                                                             \
namespace umba                                                                                    \
{                                                                                                 \
    typedef ::umba::TestFunction TestFunction;                                                    \
                                                                                                  \
    static const char groupName[] = {group_name};                                                 \
                                                                                                  \
    static int runLocalGroup(void);                                                               \
                                                                                                  \
    static void setup(void);                                                                      \
                                                                                                  \
    static void teardown(void);                                                                   \
                                                                                                  \
    static TestFunction tests[ ::umba::tests_in_group_max ];                                      \
                                                                                                  \
    static const char * testNames[ ::umba::tests_in_group_max ];                                  \
                                                                                                  \
    static uint32_t addedTests = 0;                                                               \
                                                                                                  \
    static void addTestToGroup(TestFunction test, const char * testName)                          \
    {                                                                                             \
        /* группа добавляется в список групп */                                                   \
        if(addedTests == 0)                                                                       \
        {                                                                                         \
            ::umba::addTestGroupRunner(runLocalGroup);                                            \
        }                                                                                         \
                                                                                                  \
        if(addedTests >= ::umba::tests_in_group_max)                                              \
        {                                                                                         \
            printf("\nToo much tests in one group! Increase tests_in_group_max constant.");       \
                                                                                                  \
            while(1) { if(false) return; }                                                        \
        }                                                                                         \
                                                                                                  \
        tests[addedTests] = test;                                                                 \
        testNames[addedTests] = testName;                                                         \
        addedTests++;                                                                             \
    }                                                                                             \
                                                                                                  \
    UMBA_LOCAL_GROUP_RUNNER                                                                       \
                                                                                                  \
                                                                                                  \
}


// вспомогательные макросы
#define UMBA_CONCAT2(x, y)  x ## y
#define UMBA_CONCAT(x, y) UMBA_CONCAT2(x,y)


#define UMBA_TEST( name )   namespace umba{ static const char * UMBA_CONCAT(doTest, __LINE__)(const char * umbaTestName);  }    \
                            namespace {                                                                                         \
                            class UMBA_CONCAT(UmbaTest_, __LINE__){                                                             \
                                public:                                                                                         \
                                                                                                                                \
                                UMBA_CONCAT(UmbaTest_, __LINE__)(){                                                             \
                                    /* добавляем тест в текущую группу */                                                       \
                                    umba::addTestToGroup(umba::UMBA_CONCAT(doTest, __LINE__), name);                            \
                                }                                                                                               \
                                                                                                                                \
                                                                                                                                \
                            };                                                                                                  \
                            static UMBA_CONCAT(UmbaTest_, __LINE__) UMBA_CONCAT(umbaTest_, __LINE__);                           \
                            }                                                                                                   \
                            static const char * umba::UMBA_CONCAT(doTest, __LINE__)(const char * umbaTestName)



// подвисание по-умолчанию включается
#ifndef UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED
    #define UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED 1
#endif

#define UMBA_GET_MACRO( _1, _2, NAME, ...) NAME


// основной макрос для проверок в тестах
// Если проверяемое условие ложно, то он либо подвисает, либо прокидывает сообщение об ошибке наверх

// c помощью небольшой магии его можно вызывать как с одним аргументом - условием для проверки
// так и с двумя - условием и текстом сообщения

#define UMBA_CHECK(...)    UMBA_GET_MACRO(__VA_ARGS__, UMBA_CHECK_WITH_TEXT, UMBA_CHECK_NO_TEXT) (__VA_ARGS__)

// версия проверки без сообщения об ошибке
#define UMBA_CHECK_NO_TEXT( test ) UMBA_CHECK_WITH_TEXT( test, "CHECK FAILED")

#if UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED == 1

    // fputs вместо printf, чтобы задавить warning. if(false) чтобы задавить warning
    #define UMBA_CHECK_WITH_TEXT(test, message)    do                                               \
                                                   {                                                \
                                                       if (!(test))                                 \
                                                       {                                            \
                                                           printf("\nFailure in test: ");           \
                                                                                                    \
                                                           fputs(umbaTestName, stdout);             \
                                                                                                    \
                                                           printf("\n");                            \
                                                           printf(message);                         \
                                                                                                    \
                                                           UMBA_TEST_DISABLE_IRQ();                 \
                                                           while(1) { __BKPT(0xAB);                 \
                                                                      if(false) return message; }   \
                                                        }                                           \
                                                   } while (0)

// или не подвисает
#else

    // fputs вместо printf, чтобы задавить warning
    #define UMBA_CHECK_WITH_TEXT(test, message)      do                         \
                                                     {                          \
                                                         if (!(test))           \
                                                         {                      \
                                                             return message;    \
                                                         }                      \
                                                     } while (0)

#endif

