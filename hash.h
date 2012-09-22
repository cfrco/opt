/*
 * Generic Hash Table Libaray
 * Author : cfrco (z82206.cat[at]gmail.com) 
 */
 
/*
 * Example :
 * #include<stdio.h>
 * #include"hash.h"
 *  
 * int main()
 * {
 *       hash_table_t table;
 *       hash_table_new(&table,NULL,29);  //table size=29 (%29)
 * 
 *       pairbuf_init(100,NULL);  // pairbuf allocate 100 hash_pair_t
 *     
 *       hash_table_add_str(&table,"age",10);
 *       hash_table_add_kv(&table,HASH_KEY_STR("name"),HASH_VALUE_STR("cat"));
 *       hash_table_add_str(&table,"height",1.68f);
 * 
 *       hash_pair_t *p = hash_table_get(&table,HASH_KEY_STR("age"));
 *       printf("%d\n",p->value.Int);
 * 
 *       p = hash_table_get(&table,HASH_KEY_STR("name"));
 *       printf("%s\n",p->value.Str);
 * 
 *       p = hash_table_get(&table,HASH_KEY_STR("height"));
 *       printf("%f\n",HASH_VALUE_TRANS(p->key,float));
 * 
 *       p = hash_table_get(&table,HASH_KEY_STR("wow"));
 *       // p = NULL;
 * 
 *       return 0;
 * }
 */

/*
 * Intro :
 *  Type:
 *      hash_key_t
 *      hash_value_t
 *
 *      hash_pair_t
 *      hash_table_t
 *
 *      struct pairbuf_t
 *
 *  Function:
 *      hash_do(key_t,size)
 *      
 *      hash_table_new(tablep,buf,size)
 *      hash_table_destroy(tablep)
 *      hash_table_add_pair(tablep,pair_tp)
 *      hash_table_add_kv(tablep,key_t,value_t)$
 *      hash_table_set(tablep,key_t,value_t)
 *      hash_table_get(tablep,key_t)
 *      hash_table_remove(tablep,key_t)
 *      
 *      hash_bucket_find(pair_tp,key_t)
 *
 *      hash_key_ref(ptr,size)
 *      HASH_VALUE_STR(str)
 *
 *      pairbuf_init(size,ptr)
 *      pairbuf_get()
 *      pairbuf_free(pair_tp)
 *
 *  Macro:
 *      HASH_KEY_STR(str)
 *      HASH_KEY_REF(ref)
 *      HASH_KEY_VALUE(key_t,val)
 *
 *      HASH_VALUE_TRANS(value_t,type)
 *
 *      hash_table_add_str(tablep,str,_v)$
 *      hash_table_add_ref(tablep,ref,_v)$
 *      hash_table_add_val(tablep,val,_v)$
 *
 *      hash_table_set_str(tablep,str,_v)
 *      hash_table_set_ref(tablep,ref,_v)
 *      hash_table_set_val(tablep,val,_v)
 *
 *  $ : need __PAIRBUF
 */

/*
 * Warning :
 * - Don't use different mod for hash_key_t in one hash_table.
 * - Use different type in one hash_table may cause some problem,
 *   when two different things have the same bit pattern.
 * - Sure what's type of value,or don't use different type in one hash_table.
 * - When using high-level operation,sure __PAIRBUF have enough memory space.
 * - Never trust this library.XD
 */

#ifndef _C_HASH_H_
#define _C_HASH_H_

#include<stdint.h>
#include<stddef.h>
#include<string.h>  //for strlen

enum HASH_MOD
{
    HASH_MOD_VALUE,
    HASH_MOD_REF
};

typedef struct hash_key_t
{
    union{
        void * Ptr;
        uint64_t Int64; // It means it can hold 8bytes
    } key;

    enum HASH_MOD mod; 
    size_t len;
}hash_key_t;

typedef union hash_value_t
{
    //It can hold 8bytes
    void *Ptr;
    uint64_t uInt64;
    int64_t Int64;
    int Int;
    double Double;
    char *Str;
}hash_value_t;

typedef struct hash_pair_t
{
    struct hash_key_t key;
    union hash_value_t value;
    struct hash_pair_t *next;
}hash_pair_t;

typedef struct hash_table_t
{
    struct hash_pair_t **bucket;
    size_t size;
    char flag;
}hash_table_t;

#define HASH_KEY_REF(_v) hash_key_ref((_v),sizeof(*(_v)))
#define HASH_KEY_STR(_v) hash_key_ref((void*)(_v),strlen(_v))
#define HASH_KEY_VALUE(_k,_v) do{\
    *(typeof(_v)*)&((_k).key.Ptr) = _v;\
    _k.len = sizeof(_v);\
    _k.mod = HASH_MOD_VALUE;\
}while(0)

#define HASH_VALUE_TRANS(_v,_t) (*(_t *)&(_v).Ptr)

size_t hash_do(hash_key_t hash_key,size_t hash_size);

hash_pair_t* hash_bucket_find(hash_pair_t *start,hash_key_t key);

void hash_table_new(hash_table_t *table,void *ptr,size_t hash_size);
void hash_table_destroy(hash_table_t *table);

void hash_table_add_pair(hash_table_t *table,hash_pair_t *pair);
void hash_table_set(hash_table_t *table,hash_key_t key,hash_value_t value); 
hash_pair_t* hash_table_remove(hash_table_t *table,hash_key_t key);
hash_pair_t* hash_table_get(hash_table_t *table,hash_key_t key);

static inline hash_key_t hash_key_ref(void *ptr,size_t len)
{
    hash_key_t k;
    k.key.Ptr = ptr;
    k.mod = HASH_MOD_REF;
    k.len = len;

    return k;
}

static inline hash_value_t HASH_VALUE_STR(void *ptr)
{
    hash_value_t v;
    v.Ptr = ptr;
    return v;
}

/*
 * __PAIRBUF (public,extern)
 * It's a buffer for hash_pair_t.
 * It can avoid preparing hash_pair_t everytime.
 *
 * A hash_pair_t should use pairbuf_free() to free it,
 * if you don't use it again.
 */
struct pairbuf_t
{
    hash_pair_t *buf;
    hash_pair_t **unused;
    char flag;
    size_t unused_top;
    size_t size;
};

extern struct pairbuf_t __PAIRBUF;
void pairbuf_init(size_t size,void *ptr);
hash_pair_t *pairbuf_get();
void pairbuf_free(hash_pair_t *ptr);
void pairbuf_destroy();

/*
 * There are some high-level functions.
 * But user must have to call pairbuf_inif(),before use these.
 * Because they need use __PAIRBUF
 */

static inline void hash_table_add_kv(hash_table_t *table,hash_key_t key,hash_value_t value)
{
    hash_pair_t *p = pairbuf_get();
    p->key = key;
    p->value = value;
    hash_table_add_pair(table,p);
}

#define hash_table_add_ref(_table,_key,_value) do{\
    hash_pair_t *p = pairbuf_get();\
    *(typeof(_value)*)(&p->value.Ptr) = _value;\
    p->key = HASH_KEY_REF(_key);\
    hash_table_add_pair((_table),p);\
}while(0)

#define hash_table_add_str(_table,_key,_value) do{\
    hash_pair_t *p = pairbuf_get();\
    *(typeof(_value)*)(&p->value.Ptr) = _value;\
    p->key = HASH_KEY_STR(_key);\
    hash_table_add_pair((_table),p);\
}while(0)

#define hash_table_add_val(_table,_key,_value) do{\
    hash_pair_t *p = pairbuf_get();\
    *(typeof(_value)*)(&p->value.Ptr) = _value;\
    *(typeof(_key)*)(&p->key.key.Ptr) = _key;\
    p->key.mod = HASH_MOD_VALUE;\
    p->key.len = sizeof(_key);\
    hash_table_add_pair((_table),p);\
}while(0)

#define hash_table_set_ref(_table,_key,_value) do{\
    hash_value_t value;\
    *(typeof(_value)*)(&value.Ptr) = _value;\
    hash_table_set((_table),HASH_KEY_REF(_key),value);\
}while(0)

#define hash_table_set_str(_table,_key,_value) do{\
    hash_value_t value;\
    *(typeof(_value)*)(&value.Ptr) = _value;\
    hash_table_set((_table),HASH_KEY_STR(_key),value);\
}while(0)

#define hash_table_set_val(_table,_key,_value) do{\
    hash_value_t value;\
    hash_key_t key;\
    *(typeof(_value)*)(&value.Ptr) = _value;\
    *(typeof(_key)*)(&key.key.Ptr) = _key;\
    key.len = sizeof(_key);\
    key.mod = HASH_MOD_VALUE;\
    hash_table_set((_table),key,value);\
}while(0)

#endif

