/***************************************************************************************************
                              Это файл, который затыкает собой щели.
 
 В нем лежит atexit, assert_failed и обвязка для printf'a
 
***************************************************************************************************/

#include "project_config.h"

extern "C" 
{

    /**************************************************************************************************
    Описание:  Эта функция вызывается в конструкторе каждого объекта со статическим временем жизни,
               чтобы "зарегистрировать" в динамически выделяемом списке. После выхода из main, вызовется
               _sys_exit, который для всех этих объектов вызывает деструкторы. Поскольку у нас main 
               никогда не завершается, все это абсолютно бессмысленно и только зря память жрет.
               ARM официально разрешает переопределить эту функцию.
    Аргументы: Какие-то есть, но игнорируются.
    Возврат:   Положительное число означает, что все хорошо.
    Замечания: Гуглите __aeabi_atexit и читайте официальную доку от ARM, если хотите узнать больше.
               Актуально только для ARMCC
    **************************************************************************************************/
    int __aeabi_atexit(void)
    {
        return 1;
    }
    // эти atexit'ы тоже позволяют регистрировать функции, которые вызовуться после выхода из main
    // они тоже бессмысленные
    // ненулевой возврат означает, что регистрация не удалась - и пес с ней
    // их переопределение позволяет сэкономить несколько десятков байт
    int __cxa_atexit(void)
    {
        return 1;
    }
    


    /**************************************************************************************************
    Описание:  Эта функция вызывается, если ассерт в периферийной библиотеке не смог.
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
             
            (void)file;
            (void)line;

            while(1) { __BKPT(0xAA); }
        }
    #endif

    // варианты для миландра 
    #if (USE_ASSERT_INFO == 1)    
        void assert_failed(uint32_t file_id, uint32_t line)
        {
            (void)file_id;
            (void)line;
        
            while(1) { __BKPT(0xAA); }
        }
    #elif (USE_ASSERT_INFO == 2)

        void assert_failed(uint32_t file_id, uint32_t line, const uint8_t * expr)
        {
            (void)file_id;
            (void)line;
            (void)expr;
        
            while(1) { __BKPT(0xAA); }
        }
    #endif 

}

/***************************************************************************************************
  Следующий блок функций делает возможным использования printf в симуляторе keil'a.
  Чтобы выключить - объявите символ UMBA_DONT_USE_RETARGET.
  
  Данная реализация является минимальной и не будет работать вне симулятора.
  
***************************************************************************************************/
#ifndef UMBA_DONT_USE_RETARGET 

    // если компилятор - armcc или keil-clang
    #if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

        #if ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

            asm(".global __use_no_semihosting_swi\n");
            
        #elif __CC_ARM
        
            #pragma import(__use_no_semihosting_swi)
            
            namespace std { struct __FILE { int handle;} ; }
        
        #endif
        
        #include <stdio.h>
        #include <rt_sys.h>
        #include <rt_misc.h>


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
            
            char *_sys_command_string(char *cmd, int len)
            {
                return NULL;
            }
            
            // вызывается после main
            void _sys_exit(int return_code) 
            {
                while(1) { __BKPT(0xAA); }
            }        
        }
    #endif

#endif

/***************************************************************************************************
  Этот кусок нужен только для armclang  
***************************************************************************************************/

#if ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

    // это чтобы на оптимизации -О0 armclang не пытался парсить аргументы для main'a
    __asm(".global __ARM_use_no_argv");

#endif
