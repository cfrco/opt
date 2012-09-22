/*
 *  Light Option Parser 
 *  Author : cfrco (z82206.cat[at]gmail.com)
 *
 *      Dependencies:
 *          hash.h/hash.c (cfrco)
 */

/*
 *  #Option Parser Syntax
 *
 *  program -abc --file foo.txt
 *  program -abf foo.txt
 *  program -abffoo.txt
 *  progran -ffoo.txt
 *
 *  program -abc -f foo.txt
 *
 *  program --verbose
 *
 */
#ifndef _C_OPT_H_
#define _C_OPT_H_

#include<string.h>
#include"hash.h"

typedef enum opt_action_e
{
    OPT_ACTION_SET_TRUE,
    OPT_ACTION_SET_FALSE,
    OPT_ACTION_STORE,
    OPT_ACTION_STORE_INT
}opt_action_e;

/*
 * opt_option_t o = {"-v","--verbose",OPT_ACTION_SET_TRUE,"verbose","output more information"};
 */
typedef struct opt_option_t
{
    char *sht;
    char *full;
    opt_action_e action;
    char *dest;
    char *help;
    int times;      //how many times this option be triggered successfully.
}opt_option_t;

#define OPT_HASHSIZE 37
typedef struct opt_parser_t
{
    hash_table_t value_table;
    hash_table_t option_table;
    char **args;
    size_t args_len;
}opt_parser_t;

void opt_parser_new(opt_parser_t *parser,size_t op_size);
void opt_parser_init(opt_parser_t *parser,opt_option_t *options,size_t op_size);

#define opt_parser_init_arr(_parser,_options) \
    opt_parser_init(_parser,_options,sizeof(_options)/sizeof(opt_option_t))

void opt_parser_add(opt_parser_t *parser,opt_option_t *option);
int opt_parser_start(opt_parser_t *parser,int argc,char *argv[]);

void opt_parser_handle(opt_parser_t *parser,int argc,char *argv[],
        char type,int *i,int *j,char *flag);

int opt_parser_get_value_int(opt_parser_t *parser,const char *dest);
const char* opt_parser_get_value_str(opt_parser_t *parser,const char *dest);
int opt_parser_has_value(opt_parser_t *parser,const char *dest);

opt_option_t* opt_parser_get_option(opt_parser_t *parser,const char* opt_str);

#define OPT_FLAG_SUCCESS 0
#define OPT_FLAG_NO_OPTION 1
#define OPT_FLAG_NO_VALUE 2
#define OPT_FLAG_VALUE_INVALID 3

#endif
