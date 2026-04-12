#include <stdio.h>
#include <stdarg.h>
#include "my_basic.h"

int meprint(struct mb_interpreter_t *bas,const char *fmt,...)
{
    va_list arg;
    char buff[1000];
    int ret;

    va_start(arg,fmt);
    ret=vsnprintf(buff,sizeof(buff),fmt,arg);
    va_end(arg);
    printf("\33[32m%s\33[m",buff);
    return ret;
}

int MyFn(struct mb_interpreter_t *bas, void **arg)
{
    int a,b;

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_int(bas,arg,&a));
    mb_check(mb_pop_int(bas,arg,&b));
    mb_check(mb_attempt_close_bracket(bas,arg));
    mb_check(mb_push_int(bas,arg,100));

printf("\33[33mMyFn %d,%d\33[m\n",a,b);

    return MB_FUNC_OK;
}

int main(void)
{
    struct mb_interpreter_t *bas;

    mb_init();

    bas=NULL;
    mb_open(&bas);

    mb_reg_fun(bas,MyFn);

    if(mb_load_file(bas,"test.bas")>0)
    {
        printf("Failed to load\n");
    }

//    if(mb_load_string(bas,
//            "print(\"Hello\");"
//            "print(\"Hello\");"
//            ,true)!=0)
//    {
//        printf("Load failed\n");
//    }
    else
    {
        mb_set_printer(bas,meprint);

        if(mb_run(bas,true)>0)
        {
            printf("Run failed\n");
        }
    }

    mb_close(&bas);
    mb_dispose();
    return 0;
}
