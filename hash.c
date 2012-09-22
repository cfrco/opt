/*
 * Generic Hash Table Libaray
 * Author : cfrco (z82206.cat[at]gmail.com) 
 */

#include"hash.h"
#include<stdlib.h>

struct pairbuf_t __PAIRBUF={.flag=0};

size_t hash_do(hash_key_t hash_key,size_t hash_size)
{
    uint32_t *hash_target,temp,out;
    size_t len = hash_key.len;

    if(hash_key.mod == HASH_MOD_VALUE)
        hash_target = (uint32_t*)(&hash_key.key.Int64);
    else if(hash_key.mod == HASH_MOD_REF)
        hash_target = (uint32_t*)hash_key.key.Ptr;

    out = 0;
    
    //get first 4byte
    if(len>3) out |= *(((uint8_t*)hash_target)+3)<<24;
    if(len>2) out |= *(((uint8_t*)hash_target)+2)<<16;
    if(len>1) out |= *(((uint8_t*)hash_target)+1)<<8;
    if(len>0) out |= *(((uint8_t*)hash_target));

    while(len!=0)
    {
        if(len<4)
        {
            temp = 0;
            if(len>2) temp |= *(((uint8_t*)hash_target)+2)<<16;
            if(len>1) temp |= *(((uint8_t*)hash_target)+1)<<8;
            if(len>0) temp |= *(((uint8_t*)hash_target));
            len = 0;
        }
        else
        {
            temp = *hash_target;
            len -= 4;
        }
        
        temp += ~(out<<13);
        out ^=  (temp>>8);
        temp +=  (out<<5);
        out ^=  (temp>>3);
        temp += ~(out<<7);
        out ^=  (temp>>11);

    }

    return (size_t)(out)%hash_size;
}

void hash_table_new(hash_table_t *table,void *ptr,size_t hash_size)
{
    if(ptr==NULL)
    {
        ptr = (hash_pair_t**)malloc(sizeof(hash_pair_t*)*hash_size);
        table->flag = 1;
    }
    else table->flag = 0;

    table->bucket = ptr;
    size_t i;
    for(i=0;i<hash_size;++i)
        table->bucket[i] = NULL;
    table->size = hash_size;
}

void hash_table_destroy(hash_table_t *table)
{
    size_t i;
    
    //pairbuf_free for using __PAIRBUF
    if(__PAIRBUF.flag==2)
    {
        for(i=0;i<table->size;++i)
        {
            hash_pair_t *start = table->bucket[i],*t; 

            while(start)
            {
                t = start->next;
                pairbuf_free(start); 
                start = t;
            }
        }
    }
    
    //free bucket
    if(table->flag) free(table->bucket); 
}

#define _MASK(_u,_s) ((_u)&(~((0xffffffffffffffffull)<<(_s*8))))

char hash_key_cmp(hash_key_t *a,hash_key_t *b)
{
    if(a->mod == HASH_MOD_VALUE)
        return (_MASK(a->key.Int64,a->len)==_MASK(b->key.Int64,b->len));
    
    if(a->len != b->len)
        return 0; //False

    size_t i,l=a->len;
    uint8_t *ka = a->key.Ptr;
    uint8_t *kb = b->key.Ptr;

    for(i=0;i<l;++i)
        if(*ka++!=*kb++)return 0;//False
    return 1;//True
}

hash_pair_t* hash_bucket_find(hash_pair_t *start,hash_key_t key)
{
    while(start)
    {
        if(hash_key_cmp(&start->key,&key))
           return start;
        start = start->next;
    }
    return NULL;
}

void hash_table_add_pair(hash_table_t *table,hash_pair_t *pair)
{
    if(pair==NULL)return;

    size_t hash_index = hash_do(pair->key,table->size);
    
    if(hash_bucket_find(table->bucket[hash_index],pair->key)==NULL)
    {
        pair->next = table->bucket[hash_index];
        table->bucket[hash_index] = pair;
    }
}

void hash_table_set(hash_table_t *table,hash_key_t key,hash_value_t value)
{
    size_t hash_index = hash_do(key,table->size);
    
    hash_pair_t *p = hash_bucket_find(table->bucket[hash_index],key);
    if(p!=NULL) p->value = value;
}

hash_pair_t* hash_table_remove(hash_table_t *table,hash_key_t key)
{
    size_t hash_index = hash_do(key,table->size);
    
    hash_pair_t *start=table->bucket[hash_index],*prev=NULL;
    while(start)
    {
        if(hash_key_cmp(&start->key,&key)) break;
        prev = start;
        start = start->next;
    }

    if(start!=NULL)
    {
        if(prev==NULL)table->bucket[hash_index] = start->next; 
        else prev->next = start->next;
        return start;
    }
    return NULL;
}

hash_pair_t* hash_table_get(hash_table_t *table,hash_key_t key)
{
    size_t hash_index = hash_do(key,table->size);
    return hash_bucket_find(table->bucket[hash_index],key);
}

void pairbuf_init(size_t size,void *ptr)
{
    /*
     *  __PAIRBUF.flag = 0 : None
     *                 = 1 : Use outer buffer(should free by user)
     *                 = 2 : Use malloc() allocate memory
     */

    __PAIRBUF.flag = 0;
    if(ptr==NULL)
    {
        __PAIRBUF.buf = (hash_pair_t *)malloc(size*sizeof(hash_pair_t));
        __PAIRBUF.flag++;
    }
    else __PAIRBUF.buf = ptr;

    __PAIRBUF.size = size;

    size_t i;

    __PAIRBUF.unused = (hash_pair_t **)malloc(size*sizeof(hash_pair_t*));

    for(i=0;i<size;++i)
        __PAIRBUF.unused[i] = __PAIRBUF.buf+i;

    __PAIRBUF.unused_top = size;
    __PAIRBUF.flag++;
}

hash_pair_t *pairbuf_get()
{
    if(__PAIRBUF.unused_top > 0)
        return __PAIRBUF.unused[--__PAIRBUF.unused_top];
    return NULL;
}

void pairbuf_free(hash_pair_t *ptr)
{
    if(__PAIRBUF.unused_top < __PAIRBUF.size)
        if(ptr>=__PAIRBUF.buf && ptr < __PAIRBUF.buf+__PAIRBUF.size)
            __PAIRBUF.unused[__PAIRBUF.unused_top++];
}

void pairbuf_destroy()
{
    if(__PAIRBUF.flag==2)
    {
        free(__PAIRBUF.unused);
        free(__PAIRBUF.buf);
    }

    __PAIRBUF.flag = 0;
}
