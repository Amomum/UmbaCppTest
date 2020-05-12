#include "project_config.h"
#include "umba_cpp_tests.h"

int main(void)
{
    #ifdef USE_TESTS
    
        umba::runAllTests();

        #ifdef USE_DOCKER
            return 0;
        #else
            while(1);
        #endif
        
    #endif
    
    
    return 0;
}
