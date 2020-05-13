#include "project_config.h"
#include "umba_cpp_tests.h"

int main(void)
{

    // TODO добавь макросы для ловли ассертов

    #ifdef USE_TESTS
    
        auto res = umba::runAllTests();
        (void)res;

        #ifdef USE_DOCKER
            return res;
        #else
            while(1);
        #endif
        
    #endif
    
    
    return 0;
}
