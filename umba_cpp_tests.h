/***************************************************************************************************
  Это основной заголовочный файл "фреймворка" UMBA_CPP_TESTS.
  

  Использование:

      - создайте группу тестов с помощью макроса UMBA_TEST_GROUP( "group name" )

      - создайте функции setup и teardown с помощью макросов UMBA_TEST_SETUP и UMBA_TEST_TEARDOWN.
        setup будет запускаться перед каждым тестом, teardown - после каждого теста.

      - создайте тест с помощью макроса UMBA_TEST("test name"); тест должен возвращать 0

      - для проверок внутри теста используйте макрос UMBA_CHECK( condition, "description" )

      - для тестирования потоков ОС используйте функции setTaskCycleCounter, decrementTaskCycleCounter,
        setTaskCycleCallback. "Бесконечный" цикл внутри потока должен быть
        аля while( decrementTaskCycleCounter() ) - тогда можно будет контролировать количество
        оборотов с помощью setTaskCycleCounter

      - если вы хотите проверять что-то внутри вспомогательной функции, то:
        - такая функция должна возвращать const char *
        - для проверки внутри нужно использовать UMBA_CHECK_F
        - вызов функции нужно обернуть в UMBA_CHECK_CALL


  Настройки:

      - должны лежать в файле umba_cpp_test_config.h в namespace umba

      - const uint32_t tests_in_group_max - максимальное количество тестов в одной группе

      - const uint32_t groups_max - максимальное количество групп тестов

      - переопределение типа TaskCycleCallback; это должно быть что-то, вызываемое через () без параметров

      опциональные дефайны описаны ниже

***************************************************************************************************/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "umba_cpp_test_config.h"

/***************************************************************************************************
                                  Настройки

 Поскольку "фреймворк" ориентирован в первую очередь на симулятор Keil'a для Cortex-M3,
 по-умолчанию форсятся соответствующие настройки

***************************************************************************************************/

// включает логирование (т.е. каждый тест рапортует о запуске)
#ifndef UMBA_TEST_LOGGING_ENABLED
    #define UMBA_TEST_LOGGING_ENABLED    0
#endif

// останавливать ли отладчик на упавшем тесте
// остановка происходит с помощью макроса UMBA_TEST_STOP_DEBUGGER
#ifndef UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED
    #define UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED   1
#endif

// макрос для выключения прерываний, чтобы прерывания не выдергивали из ассерта
#ifndef UMBA_TEST_DISABLE_IRQ
    #define UMBA_TEST_DISABLE_IRQ()     __disable_irq()
#endif

// макрос для остановки отладчика - в Cortex-M для этого есть специальная инструкция
#ifndef UMBA_TEST_STOP_DEBUGGER
    #define UMBA_TEST_STOP_DEBUGGER()   __BKPT(0xAA)
#endif

// использование исключений в ассертах - чтобы ассерт можно было протестировать
#ifndef UMBA_TEST_ENABLE_ASSERT_EXCEPTIONS
    #define UMBA_TEST_ENABLE_ASSERT_EXCEPTIONS  0
#endif

// заменять ли статический ассерт на рантаймовый - опять же, чтобы его можно было протестировать
#ifndef UMBA_USE_RUNTIME_STATIC_ASSERT
    #define UMBA_USE_RUNTIME_STATIC_ASSERT   0
#endif

/***************************************************************************************************

***************************************************************************************************/

// функция, которая будет вызываться перед каждым тестом
#define UMBA_TEST_SETUP()    static void umba::setup(void)

// функция, которая будет вызываться после каждого теста
#define UMBA_TEST_TEARDOWN() static void umba::teardown(void)

// вспомогательный макрос на случай, если вам нужно возвращать типизированный ноль
// например, чтобы помочь выводу типов
#define UMBA_TEST_OK (const char *)(0)

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

    // установить счетчик требуемого количества проходов основного цикла задачи при тестировании
    void setTaskCycleCounter(uint32_t cnt);

    // вызов этой функции должен быть в условии "бесконечного" цикла потока
    uint32_t decrementTaskCycleCounter(void);

    // этот коллбек вызывается внутри decrementTaskCycleCounter
    // т.е. на каждом обороте цикла
    void setTaskCycleCallback( TaskCycleCallback cb );

    // вспомогательное
    template< typename T, size_t N >
    inline size_t getArraySize( T (&)[N] )
    {
        return N;
    }

} // namespace umba




#if UMBA_TEST_LOGGING_ENABLED == 1

    #define UMBA_LOCAL_GROUP_RUNNER                                                      \
                                                                                         \
    static int runLocalGroup(void)                                                       \
    {                                                                                    \
        int groupResult = 0;                                                             \
                                                                                         \
        uint8_t maxTestNameSize = 0;                                                     \
        for ( uint8_t i = 0; i < addedTests; i++ )                                       \
        {                                                                                \
            if ( namesSize[i] > maxTestNameSize )                                        \
            {                                                                            \
                maxTestNameSize = namesSize[i];                                          \
            }                                                                            \
        }                                                                                \
                                                                                         \
        printf("\nStarting tests in group \"%s\":\n", groupName);                        \
                                                                                         \
        for(uint32_t i=0; i<addedTests; i++)                                             \
        {                                                                                \
                                                                                         \
            ::umba::incrementTestsRun();                                                 \
            setup();                                                                     \
                                                                                         \
            printf("Test -- %s ", testNames[i]);                                         \
                                                                                         \
            const char * result = tests[i]( testNames[i] );                              \
                                                                                         \
            teardown();                                                                  \
                                                                                         \
            /* какой-то тест провалился - выводим сообщение об ошибке */                 \
            /* в режиме с остановкой - тест сам выведет сообщение*/                      \
            if( result != 0)                                                             \
            {                                                                            \
                const char * fail = "...FAIL\n";                                         \
                uint8_t offset = maxTestNameSize - namesSize[i] + 7;                     \
                printf("%s%*s%s", ::umba::RED_FG, offset, fail, ::umba::COLOR_RESET);    \
                                                                                         \
                printf("\n\n%sIn group \"%s\" failed test \"%s\":%s \n",                 \
                        ::umba::RED_BG, groupName, testNames[i],::umba::COLOR_RESET);    \
                printf("%s%s%s \n\n", ::umba::RED_BG, result, ::umba::COLOR_RESET);      \
                groupResult = 1;                                                         \
            }                                                                            \
            else                                                                         \
            {                                                                            \
                const char * ok = "...OK\n";                                             \
                uint8_t offset = maxTestNameSize - namesSize[i] + 6;                     \
                printf("%s%*s%s", ::umba::GREEN_FG, offset, ok, ::umba::COLOR_RESET);    \
            }                                                                            \
                                                                                         \
        }                                                                                \
                                                                                         \
        return groupResult;                                                              \
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
            /* Чтобы Кейл предупреждение про неиспользуемый статический массив не кидал */        \
            volatile uint8_t dummy = namesSize[i];                                                \
            (void)dummy;                                                                          \
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

/***************************************************************************************************
               Макрос для создания группы тестов

 - group_name - название группы, произвольный строковый литерал
***************************************************************************************************/
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
#define UMBA_TEST_CONCAT2(x, y)  x ## y
#define UMBA_TEST_CONCAT(x, y) UMBA_TEST_CONCAT2(x,y)

/***************************************************************************************************
               Макрос для создания теста

 - name - название теста, произвольный строковый литерал

 Тест должен возвращать 0 (или nullptr, или UMBA_TEST_OK), чтобы показать, что он пройден
***************************************************************************************************/
#define UMBA_TEST( name )                                                                                 \
    namespace umba{ static const char * UMBA_TEST_CONCAT(doTest, __LINE__)(const char * umbaTestName);  } \
    namespace {                                                                                           \
    class UMBA_TEST_CONCAT(UmbaTest_, __LINE__){                                                          \
        public:                                                                                           \
                                                                                                          \
        UMBA_TEST_CONCAT(UmbaTest_, __LINE__)(){                                                          \
            /* добавляем тест в текущую группу */                                                         \
            umba::addTestToGroup( umba::UMBA_TEST_CONCAT(doTest, __LINE__),                               \
                                  name, ::umba::getArraySize(name));                                      \
        }                                                                                                 \
                                                                                                          \
                                                                                                          \
    };                                                                                                    \
    static UMBA_TEST_CONCAT(UmbaTest_, __LINE__) UMBA_TEST_CONCAT(umbaTest_, __LINE__);                   \
    }                                                                                                     \
    static const char * umba::UMBA_TEST_CONCAT(doTest, __LINE__)(const char * umbaTestName)




// вспомогательный макрос для выбора одно- или двухпараметричного макроса
#define UMBA_TEST_GET_MACRO( _1, _2, NAME, ...) NAME


/***************************************************************************************************
               Макрос для проверок прямо в теле теста - UMBA_CHECK

 Если проверяемое условие ложно, то он либо подвисает, либо прокидывает сообщение об ошибке наверх

 c помощью небольшой магии его можно вызывать как с одним аргументом - условием для проверки:
     UMBA_CHECK( false )

 так и с двумя - условием и текстом сообщения
     UMBA_CHECK( false, "false is false" )

***************************************************************************************************/

// DUMMY нужен, чтобы убрать ворнинг про "ISO C++11 requires at least one argument for the "..." in a variadic macro"
#define UMBA_CHECK(...)  UMBA_TEST_GET_MACRO(__VA_ARGS__, UMBA_CHECK_WITH_TEXT, UMBA_CHECK_NO_TEXT, DUMMY ) (__VA_ARGS__)

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
             printf("%s", umbaTestName);              \
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



/***************************************************************************************************
               Макрос для проверок в вспомогательной функции

 Если проверяемое условие ложно, то он либо подвисает, либо прокидывает сообщение об ошибке наверх.
 Вызов проверяемой функции должен быть обернут в UMBA_CHECK_CALL( function() )

 c помощью небольшой магии его можно вызывать как с одним аргументом - условием для проверки:
     UMBA_CHECK_F( false )

 так и с двумя - условием и текстом сообщения
     UMBA_CHECK_F( false, "false is false" )

***************************************************************************************************/

#define UMBA_CHECK_F(...)             UMBA_TEST_GET_MACRO(__VA_ARGS__, UMBA_CHECK_F_WITH_TEXT, UMBA_CHECK_F_NO_TEXT, DUMMY ) (__VA_ARGS__)
#define UMBA_CHECK_F_NO_TEXT(test)    UMBA_CHECK_F_WITH_TEXT( test, "fail")

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

/***************************************************************************************************
               Макрос, в который нужно оборачивать вызов вспомогательной функции с проверкой
               
 const char * checkSomething()
 {
    UMBA_CHECK_F( a == b, "blah blah" );
    return 0;
 }

 ...

 UMBA_CHECK_CALL( checkSomething() ); 
               
***************************************************************************************************/
#define UMBA_CHECK_CALL( call ) { const char * r = call; UMBA_CHECK( r == UMBA_TEST_OK, r ); }


/***************************************************************************************************
               Макрос для ассерта - UMBA_ASSERT

 Если проверяемое условие ложно, то он:
    - если включено UMBA_TEST_ENABLE_ASSERT_EXCEPTIONS - то кидает исключение, которое можно поймать
      с помощью UMBA_CHECK_ASSERTION

    - если включено UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED, то останавливает отладчик
      с помощью UMBA_TEST_DISABLE_IRQ и UMBA_TEST_STOP_DEBUGGER

    - иначе - дергает UMBA_TEST_USER_DEFINED_ASSERT

***************************************************************************************************/
#if UMBA_TEST_ENABLE_ASSERT_EXCEPTIONS == 1

    #define UMBA_ASSERT( statement )                                               \
        do                                                                         \
        {                                                                          \
            if(! (statement) )                                                     \
            {                                                                      \
                printf("\nUmba Assertion failed in " __FILE__ ":%d\n", __LINE__ ); \
                throw ::umba::AssertionFailedException();                          \
            }                                                                      \
        } while(0)

    #include <exception>

    namespace umba
    {
        class AssertionFailedException : public std::exception
        { };
    }

    /***************************************************************************************************
               Макрос для тестирования ассертов

     Чтобы он работал, необходимо разрешить исключения и разрешить UMBA_TEST_ENABLE_ASSERT_EXCEPTIONS

    ***************************************************************************************************/

    #define UMBA_CHECK_ASSERTION( expression, message )       \
    try                                                       \
    {                                                         \
        { expression; }                                       \
                                                              \
        UMBA_CHECK( false, message );                         \
    }                                                         \
    catch( ::umba::AssertionFailedException & e )             \
    { }

#elif UMBA_TEST_STOP_DEBUGGER_ON_FAILED_TEST_ENABLED

    #define UMBA_ASSERT( statement ) \
        do { if(! (statement) ) { UMBA_TEST_DISABLE_IRQ(); while(1){ UMBA_TEST_STOP_DEBUGGER(); if(0) break;} }  } while(0)

#else
    #define UMBA_ASSERT( statement ) UMBA_TEST_USER_DEFINED_ASSERT( statement )
#endif


/***************************************************************************************************
           Макрос для статического ассерта

  - если вы хотите тестировать статические ассерты, включите UMBA_USE_RUNTIME_STATIC_ASSERT;
    в таком случае статический ассерт превратится в обычный и его можно тестировать как обычный

  - в остальных случаях превращается либо в static_assert либо в костыль для С++03

  К сожалению, для костыля в стиле С++03 msg не может быть строковым литералом, а должен быть типа
  reason_for_assert, потому что он приклеивается к имени типа

***************************************************************************************************/
#if UMBA_USE_RUNTIME_STATIC_ASSERT == 1

    #define UMBA_STATIC_ASSERT( condition, msg ) UMBA_ASSERT( condition )

#else

    #if __cplusplus < 201103L
        #define UMBA_STATIC_ASSERT( condition, msg ) typedef char umba_static_assertion_##msg[(condition)?1:-1]
        #define UMBA_STATIC_ASSERT3(X, L) UMBA_STATIC_ASSER(X, at_line_##L)
        #define UMBA_STATIC_ASSERT2(X, L) UMBA_STATIC_ASSERT3(X, L)
    #else
        #define UMBA_STATIC_ASSERT( condition, msg ) static_assert( condition, msg )
    #endif
#endif
