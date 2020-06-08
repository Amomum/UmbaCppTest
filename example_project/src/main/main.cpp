#include "project_config.h"
#include "umba_cpp_tests.h"

int main(void)
{

    #ifdef USE_TESTS
    
        #ifdef USE_DOCKER
            // Отключение буферизации помогает тестовому выводу не смешиваться с
            // выводом от санитайзеров
            // почему-то в Кейле этот вызов приводит к каким-то странным последствиям
            setvbuf(stdout, NULL, _IONBF, 0);
        #endif
        
        int res = umba::runAllTests();
        (void)res;

        #ifdef USE_DOCKER
            return res;
        #else
            while(1);
        #endif
        
    #endif
    
    
    return 0;
}
