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
#include <stddef.h>

#include "umba_cpp_test_config.h"

namespace umba
{

    typedef const char * (*TestFunction)(const char * umbaTestName);
    
    typedef int (*GroupRunner)(void);
    
    // возвращает 0, если все тесты пройдены успешно
    int runAllTests();
    
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

    template< typename T, size_t N >
    inline size_t getArraySize( T (&)[N] )
    {
        return N;
    }

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
                                                                                              \
    uint8_t maxTestNameSize = 0;                                                              \
    for ( uint8_t i = 0; i < addedTests; i++ )                                                \
    {                                                                                         \
        if ( namesSize[i] > maxTestNameSize )                                                 \
        {                                                                                     \
            maxTestNameSize = namesSize[i];                                                   \
        }                                                                                     \
    }                                                                                         \
                                                                                              \
    printf("\nStarting tests in group \"%s\":\n", groupName);                                 \
                                                                                              \
    for(uint32_t i=0; i<addedTests; i++)                                                      \
    {                                                                                         \
                                                                                              \
        ::umba::incrementTestsRun();                                                          \
        setup();                                                                              \
                                                                                              \
        printf("Test -- %s ", testNames[i]);                                                  \
                                                                                              \
        const char * result = tests[i]( testNames[i] );                                       \
                                                                                              \
        teardown();                                                                           \
                                                                                              \
        /* какой-то тест провалился - выводим сообщение об ошибке */                          \
        /* в режиме с остановкой - тест сам выведет сообщение*/                               \
        if( result != 0)                                                                      \
        {                                                                                     \
            const char * fail = "...FAIL\n";                                                  \
            uint8_t offset = maxTestNameSize - namesSize[i] + 7;                              \
            printf("%s%*s%s", ::umba::RED_FG, offset, fail, ::umba::COLOR_RESET);             \
                                                                                              \
            printf("\n\n%sIn group \"%s\" failed test \"%s\":%s \n",                          \
                    ::umba::RED_BG, groupName, testNames[i],::umba::COLOR_RESET);             \
            printf("%s%s%s \n\n", ::umba::RED_BG, result, ::umba::COLOR_RESET);               \
            groupResult = 1;                                                                  \
        }                                                                                     \
        else                                                                                  \
        {                                                                                     \
            const char * ok = "...OK\n";                                                      \
            uint8_t offset = maxTestNameSize - namesSize[i] + 6;                              \
            printf("%s%*s%s", ::umba::GREEN_FG, offset, ok, ::umba::COLOR_RESET);             \
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
    static uint8_t namesSize[ ::umba::tests_in_group_max ];                                       \
                                                                                                  \
    static uint32_t addedTests = 0;                                                               \
                                                                                                  \
    static void addTestToGroup(TestFunction test, const char * testName, uint8_t nameSize)        \
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
        namesSize[addedTests] = nameSize;                                                         \
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


#define UMBA_TEST( name )                                                                                 \
    namespace umba{ static const char * UMBA_CONCAT(doTest, __LINE__)(const char * umbaTestName);  }      \
    namespace {                                                                                           \
    class UMBA_CONCAT(UmbaTest_, __LINE__){                                                               \
        public:                                                                                           \
                                                                                                          \
        UMBA_CONCAT(UmbaTest_, __LINE__)(){                                                               \
            /* добавляем тест в текущую группу */                                                         \
            umba::addTestToGroup( umba::UMBA_CONCAT(doTest, __LINE__), name, ::umba::getArraySize(name)); \
        }                                                                                                 \
                                                                                                          \
                                                                                                          \
    };                                                                                                    \
    static UMBA_CONCAT(UmbaTest_, __LINE__) UMBA_CONCAT(umbaTest_, __LINE__);                             \
    }                                                                                                     \
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

// DUMMY нужен, чтобы убрать ворнинг про "ISO C++11 requires at least one argument for the "..." in a variadic macro"
#define UMBA_CHECK(...)  UMBA_GET_MACRO(__VA_ARGS__, UMBA_CHECK_WITH_TEXT, UMBA_CHECK_NO_TEXT, DUMMY ) (__VA_ARGS__)

// версия проверки без сообщения об ошибке
#define UMBA_CHECK_NO_TEXT( test ) UMBA_CHECK_WITH_TEXT( test, "CHECK FAILED")

#if UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED == 1

    // if(false) чтобы задавить warning
    #define UMBA_CHECK_WITH_TEXT(test, message)       \
      do                                              \
      {                                               \
         (void)umbaTestName;                          \
         if (!(test))                                 \
         {                                            \
             printf("\nFailure in test: %s ",         \
                     umbaTestName);                   \
                                                      \
             fputs(umbaTestName, stdout);             \
                                                      \
             printf("\n");                            \
             printf("%s", message);                   \
                                                      \
             UMBA_TEST_DISABLE_IRQ();                 \
             while(1)                                 \
             {                                        \
                 UMBA_TEST_STOP_DEBUGGER();           \
                 if(false) return message;            \
             }                                        \
          }                                           \
      } while (0)

// или не подвисает
#else

    // fputs вместо printf, чтобы задавить warning
    #define UMBA_CHECK_WITH_TEXT(test, message)  \
      do                                         \
      {                                          \
         (void)umbaTestName;                     \
         if (!(test))                            \
         {                                       \
             return message;                     \
         }                                       \
      } while (0)

#endif



// Добавочные макросы, чтобы чек можно было делать внутри функции

#define UMBA_CHECK_F(...)  UMBA_GET_MACRO(__VA_ARGS__, UMBA_CHECK_F_WITH_TEXT, UMBA_CHECK_F_NO_TEXT, DUMMY ) (__VA_ARGS__)
#define UMBA_CHECK_F_NO_TEXT(test)          UMBA_CHECK_F_WITH_TEXT( test, "fail")

#if UMBA_TEST_HANG_ON_FAILED_TEST_ENABLED == 1

    #define UMBA_CHECK_F_WITH_TEXT(test, message)   \
    {                                               \
        if( ! (test) )                              \
        {                                           \
           printf("%s", message);                   \
           UMBA_TEST_DISABLE_IRQ();                 \
           while(1)                                 \
           {                                        \
               UMBA_TEST_STOP_DEBUGGER();           \
               if(false) return message;            \
           }                                        \
        }                                           \
    }

#else

    #define UMBA_CHECK_F_WITH_TEXT(test, message) { if( ! (test) ) { return message; } }

#endif

#define UMBA_CHECK_CALL( call ) { auto r = call; UMBA_CHECK( r == nullptr, r ); }
