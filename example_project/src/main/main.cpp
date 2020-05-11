#include "project_config.h"
#include "umba_cpp_test/umba_cpp_tests.h"

int main(void)
{
    #ifdef USE_TESTS
    
        umba::runAllTests();    
        while(1);
        
    #endif
    
    
    return 0;
}
