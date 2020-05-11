/***************************************************************************************************
 Файл со вспомогательными функциями для UMBA_TESTS
***************************************************************************************************/

#include "umba_cpp_tests.h"

namespace umba
{


    static GroupRunner testGroupRunners[groups_max];

    static uint32_t addedGroups = 0;

     // Счетчик. Показывает, сколько всего тестов было запущено.
    static uint32_t testsRun = 0;

    void incrementTestsRun(void)
    {
        testsRun++;
    }

    uint32_t getTestsRun(void)
    {
        return testsRun;
    }




    /***************************************************************************************************
                                Глобальные функции
    ***************************************************************************************************/

    void addTestGroupRunner(GroupRunner groupRunner)
    {
        if(addedGroups >= groups_max)
        {
            printf("\nToo much groups! Please increase groups_max constant.");

            while(1) { if(false) return; }
        }

        testGroupRunners[addedGroups] = groupRunner;
        addedGroups++;
    }

    void runAllTests()
    {
        bool allOk = true;
        
        for(uint32_t i=0; i<addedGroups; i++)
        {
            int result = testGroupRunners[i]();

            if(result != 0)
            {
                allOk = false;
            }
        }

        printf("Tests run: %d\n", (int)testsRun);

        if(allOk)
        {
            printf("All tests passed!");
        }
        else
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

    }


    /**************************************************************************************************
     Счетчик требуемого количества проходов цикла задачи при тестировании и колбэк к нему
    **************************************************************************************************/

    static uint32_t taskCycleCounter = 0;
    static TaskCycleCallback taskCycleCallback;

    void setTaskCycleCounter(uint32_t cnt)
    {
        taskCycleCounter = cnt;
    }

    uint32_t decrementTaskCycleCounter(void)
    {
        if (taskCycleCallback != NULL)
        {
            taskCycleCallback();
        }
        uint8_t a = taskCycleCounter;
        taskCycleCounter--;
        return a;
    }

    void setTaskCycleCallback( TaskCycleCallback cb )
    {
        taskCycleCallback = cb;
    }


}
