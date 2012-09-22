/*
 *  Light Option Parser 
 *  Author : cfrco (z82206.cat[at]gmail.com)
 */

#include"opt.h"
#include<stdlib.h>
#include<stdio.h>

void opt_parser_new(opt_parser_t *parser,size_t op_size)
{
    if(__PAIRBUF.flag==0)
        pairbuf_init(op_size*3,NULL);
    
    hash_table_new(&parser->option_table,NULL,OPT_HASHSIZE);
    hash_table_new(&parser->value_table,NULL,OPT_HASHSIZE);

    parser->args = NULL;
    parser->args_len = 0;
}

void opt_parser_init(opt_parser_t *parser,opt_option_t *options,size_t op_size)
{
    opt_parser_new(parser,op_size);
    size_t i;

    for(i=0;i<op_size;++i)
        opt_parser_add(parser,options+i);
}

void opt_parser_add(opt_parser_t *parser,opt_option_t *option)
{
    option->times = 0;

    if(option->sht!=NULL)
        hash_table_add_str(&parser->option_table,option->sht,option);

    if(option->full!=NULL)
        hash_table_add_str(&parser->option_table,option->full,option);

    if(option->dest!=NULL)
    {
        if(option->action==OPT_ACTION_SET_TRUE)
            hash_table_add_str(&parser->value_table,option->dest,0);
        else if(option->action==OPT_ACTION_SET_FALSE)
            hash_table_add_str(&parser->value_table,option->dest,1);
        else hash_table_add_str(&parser->value_table,option->dest,NULL);
    }
}

#define OPT_TYPE_SHT 0
#define OPT_TYPE_FULL 1

void opt_parser_handle(opt_parser_t *parser,int argc,char *argv[],char type,
                       int *i,int *j,char *flag)
{
    char sht[3] = {'-',0,0};
    opt_option_t *option;
    hash_pair_t *pair;
    
    if(type == OPT_TYPE_SHT)
    {
        sht[1] = argv[*i][*j];
        pair = hash_table_get(&parser->option_table,HASH_KEY_STR(sht));
    }
    else
        pair = hash_table_get(&parser->option_table,HASH_KEY_STR(argv[*i]));

    if(pair == NULL)
    {
        *flag = OPT_FLAG_NO_OPTION; 
        return ;
    }

    option = (opt_option_t*)pair->value.Ptr;
    pair = hash_table_get(&parser->value_table,HASH_KEY_STR(option->dest));

    if(option->action == OPT_ACTION_SET_TRUE)
        pair->value.Int = 1;
    else if(option->action == OPT_ACTION_SET_FALSE)
        pair->value.Int = 0;
    else
    {
        if(type == OPT_TYPE_SHT && argv[*i][*j+1]!=0)
        {
            pair->value.Ptr = &argv[*i][*j+1]; 

            //move j to last char
            while(argv[*i][*j]!=0)*j+=1;
            *j -= 1;
        }
        else if(*i+1<argc)
        {
            pair->value.Ptr = argv[*i+1]; 
            *i += 1;
        }
        else 
        {
            *flag = OPT_FLAG_NO_VALUE; 
            return;
        }

        if(option->action == OPT_ACTION_STORE_INT)
        {
            if(!str2int(pair->value.Ptr,&pair->value.Int))
            {
                *flag = OPT_FLAG_VALUE_INVALID; 
                return;
            } 
        }
    }

    //success 
    option->times++;
}
int opt_parser_start(opt_parser_t *parser,int argc,char *argv[])
{
    int i,j,oi;
    char flag = OPT_FLAG_SUCCESS;
    parser->args = (char **)malloc(sizeof(char*)*argc);
    
    for(i=1;i<argc;++i) //i=1:avoid program name
    {
        if(argv[i][0]=='-')
        {
            //start with "--"
            if(argv[i][1]=='-')
            {
                opt_parser_handle(parser,argc,argv,OPT_TYPE_FULL,&i,&j,&flag);
            }
            //start with "-"
            else
            {
                j=1;
                while(argv[i][j]!=0)
                {
                    oi = i;
                    opt_parser_handle(parser,argc,argv,OPT_TYPE_SHT,&i,&j,&flag);

                    if(flag != OPT_FLAG_SUCCESS)break;
                    if(oi != i) break;
                    ++j;
                }
            }
        }
        else parser->args[parser->args_len++] = argv[i]; 

        if(flag!=OPT_FLAG_SUCCESS) break;
    }

    /*
     * Error Output
     * #bug : `%s` may not put out the true error option
     */
    if(flag==OPT_FLAG_NO_OPTION)
        printf("No this option `%s`.\n",argv[i]);
    else if(flag==OPT_FLAG_NO_VALUE)
        printf("No value for option `%s`.\n",argv[i]);
    else if(flag==OPT_FLAG_VALUE_INVALID)
        printf("Invalid value for option `%s`.\n",argv[i]);
    
    return flag;
}

int opt_parser_get_value_int(opt_parser_t *parser,const char *dest)
{
    hash_pair_t *pair;
    pair = hash_table_get(&parser->value_table,HASH_KEY_STR(dest));
    
    if(pair==NULL)return 0;
    return pair->value.Int;
}

const char* opt_parser_get_value_str(opt_parser_t *parser,const char *dest)
{
    hash_pair_t *pair;
    pair = hash_table_get(&parser->value_table,HASH_KEY_STR(dest));
    
    if(pair==NULL)return NULL;
    return (const char *)pair->value.Ptr;
}

int opt_parser_has_value(opt_parser_t *parser,const char *dest)
{
    hash_pair_t *pair;
    pair = hash_table_get(&parser->value_table,HASH_KEY_STR(dest));

    return pair!=NULL;
}

opt_option_t* opt_parser_get_option(opt_parser_t *parser,const char* opt_str)
{
    hash_pair_t *pair;
    pair = hash_table_get(&parser->option_table,HASH_KEY_STR(opt_str));
    
    if(pair==NULL) return NULL;
    return (opt_option_t*)pair->value.Ptr;
}
