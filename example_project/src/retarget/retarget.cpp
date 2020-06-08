/***************************************************************************************************
                              Это файл, который затыкает собой щели.
 
 В нем лежит atexit, assert_failed и обвязка для printf'a
 
***************************************************************************************************/

#include "project_config.h"

extern "C" 
{

    /**************************************************************************************************
    Описание: Эта функция вызывается на выходе из main и вызывает деструкторы статических объектов.
              Для этого она использует кучу и притягивает в проект кучу лишнего кода.
              Поскольку она все равно никогда не вызывается - мы ее заменяем на вот эту затычку.
    Аргументы: -
    Возврат:   Пес его знает
    Замечания: -
    **************************************************************************************************/
    int __aeabi_atexit(void)
    {
        return 1;
    }

    /**************************************************************************************************
    Описание: Эта функция вызывается, если ассерт в периферийной библиотеке не смог.
    Аргументы: file - имя файла, в котором ассерт сработал, line - номер строки
    Возврат:   -
    Замечания: Вызывает обычный UMBA_ASSERT и повисает.
    **************************************************************************************************/

    // классический ассерт для STM32
    #ifdef USE_FULL_ASSERT
        void assert_failed(uint8_t * file, uint32_t line)
        { 
            /* User can add his own implementation to report the file name and line number,
             ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

            while(1) { __BKPT(0xAB); }
        }
    #endif

    // варианты для миландра 
    #if (USE_ASSERT_INFO == 1)    
        void assert_failed(uint32_t file_id, uint32_t line)
        {
            while(1) { __BKPT(0xAB); }
        }
    #elif (USE_ASSERT_INFO == 2)

        void assert_failed(uint32_t file_id, uint32_t line, const uint8_t * expr);
        {
            while(1) { __BKPT(0xAB); }
        }
    #endif 

}

/***************************************************************************************************
  Следующий блок функций делает возможным использования printf в симуляторе keil'a.
  Чтобы включить - объявите символ UMBA_USE_RETARGET.
  
  Данная реализация является минимальной и не будет работать вне симулятора.
  
***************************************************************************************************/
#if defined UMBA_USE_RETARGET && __CC_ARM

    #pragma import(__use_no_semihosting_swi)

    #include <stdio.h>
    #include <rt_sys.h>
    #include <rt_misc.h>


    namespace std { struct __FILE { int handle;} ; }

    std::FILE std::__stdout;
    std::FILE std::__stdin;
    std::FILE std::__stderr;
    
    extern "C"
    { 
        int fputc(int c, FILE *f)
        {
            return ITM_SendChar(c);
        }

        int fgetc(FILE *f)
        {
          char ch = 0;

          return((int)ch);
        }

        int ferror(FILE *f)
        {
          /* Your implementation of ferror */
          return EOF;
        }

        void _ttywrch(int ch)
        {
          ITM_SendChar(ch);
        }

        void _sys_exit(int return_code) 
        {
            while(1);
        }        
    }

#endif
