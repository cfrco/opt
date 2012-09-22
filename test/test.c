#include"opt.h"
#include<stdio.h>

opt_option_t options[] = {
    {"-s","--say",OPT_ACTION_STORE,"say",NULL},
    {"-v","--verbose",OPT_ACTION_SET_TRUE,"verbose",NULL},
    {NULL,"--output",OPT_ACTION_STORE,"output",NULL},
    {"-t","--time",OPT_ACTION_STORE_INT,"time",NULL}
};

int main(int argc,char *argv[])
{
    opt_parser_t parser;
    opt_parser_init_arr(&parser,options);
    printf("init ...\n");

    char flag = opt_parser_start(&parser,argc,argv);
    printf("parse ...\n");
    char *str;

    if(flag==OPT_FLAG_SUCCESS)
    {
        printf("%s : %d\n","verbose",opt_parser_get_value_int(&parser,"verbose"));  
        str = opt_parser_get_value_str(&parser,"output");
        if(str)
            printf("%s : %s\n","output",str);
        
        str = opt_parser_get_value_str(&parser,"say");
        if(str)
            printf("%s : %s\n","say",str);

        if(opt_parser_get_option(&parser,"--time")->times>0)
            printf("%s : %d\n","time",opt_parser_get_value_int(&parser,"time"));

        int i;
        for(i=0;i<parser.args_len;++i)
            printf("%s\n",parser.args[i]);
    }

    return 0;
}
