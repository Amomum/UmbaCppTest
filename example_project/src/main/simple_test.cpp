#include "project_config.h"

#ifdef USE_TESTS

#include "umba_cpp_tests.h"

namespace
{

    UMBA_TEST_GROUP("Simple tests of simple things")

    /***************************************************************************************************
                                           Тестовые данные
    ***************************************************************************************************/
    
    /***************************************************************************************************
                                           Объекты для тестов
    ***************************************************************************************************/
    
    /***************************************************************************************************
                                                 Моки
    ***************************************************************************************************/

    /***************************************************************************************************
                                         Вспомогательные функции
    ***************************************************************************************************/

    UMBA_TEST_SETUP()
    {
        //printf("setup\n");
    }

    UMBA_TEST_TEARDOWN()
    {
        //printf("\nteardown\n");
    }
    
    void functionThatAsserts()
    {
        UMBA_ASSERT( false );
    }

    void functionThatStaticAsserts()
    {
        UMBA_STATIC_ASSERT(false, error_description_in_cpp03_style );
    }

    const char * functionThatContainsCheck( bool state )
    {
        UMBA_CHECK_F( state == false, "state should be false");

        return UMBA_TEST_OK;
    }



    /***************************************************************************************************
                                                 Тесты
    ***************************************************************************************************/

    UMBA_TEST("This test will pass")
    {
        UMBA_CHECK( true, "True is always true" );
        
        return 0;

    }

    UMBA_TEST("This test will also pass")
    {
        UMBA_CHECK( true, "True is always true" );
        return 0;
    }
    
    UMBA_TEST("Calling auxillary function that checks something - should pass")
    {
        UMBA_CHECK_CALL( functionThatContainsCheck( false ) );

        return 0;
    }

    UMBA_TEST("Calling function that asserts - should assert")
    {    
        UMBA_CHECK_ASSERTION( functionThatAsserts(), "" );

        return 0;
    }
    
    UMBA_TEST("Calling function that statically asserts - should assert")
    {
        UMBA_CHECK_ASSERTION( functionThatStaticAsserts(), "" );

        return 0;
    }

    UMBA_TEST("This test will always fail")
    {
        UMBA_CHECK( false, "Who's fault is it? It's SEGMENTATION FAULT!" );
        return 0;
    }

    UMBA_TEST("This test should trigger ASAN runtime error\n")
    {
    	volatile int * a = 0;

    	*a = 1;

        UMBA_CHECK( true, "True is always true" );
        return 0;
    }


      

} // anonymous namespace   

#endif
