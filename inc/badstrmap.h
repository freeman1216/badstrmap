#pragma once
#ifndef BAD_STR_MAP
#define BAD_STR_MAP

#include <stdint.h>
#ifdef BAD_STR_MAP_USE_STRING_H
#include <string.h>
#endif


typedef struct{
    uint8_t* data;
    uint32_t length;
} fstring_t;

typedef struct {
    uint8_t *arena;
    uint8_t *curr;
    fstring_t **keys;
    void **data; 
    uint32_t map_size;
    uint32_t count;
    uint32_t arena_size;
} bad_str_map_t;

typedef enum {
    BAD_STR_MAP_ALLOC_FAIL = 0,
    BAD_STR_MAP_OK,
    BAD_STR_MAP_ALREADY_EXISTS,
    BAD_STR_MAP_FULL
}bad_str_map_status_t;

#ifdef BAD_STR_MAP_STATIC
    #define BAD_STR_MAP_DEF static
#else 
    #define BAD_STR_MAP_DEF extern
#endif // BAD_STR_MAP_STATIC

#define BAD_STR_MAP_ALIGN (_Alignof(bad_str_map_t))
#define BAD_STR_MAP_ALIGN_ATTR __attribute__((aligned(BAD_STR_MAP_ALIGN)))

#define BAD_STR_MAP_ALLOC_SIZE(map_size,map_arena_size) ({ \
    _Static_assert((map_size & (map_size-1)) == 0,"Map size must be a power of 2"); \
    _Static_assert((map_arena_size & 0x3) == 0,"Map arena size must be a multiple of word_size" );\
    (sizeof(bad_str_map_t) + (((map_size) << 1) * 2) * (sizeof(void *)) + (map_arena_size)); \
})

#define BAD_STR_MAP_GET_KEYS_PTR(name)({\
    _Static_assert(sizeof(*(name)) == 1,"Macro requires byte ptr");\
    ((name) + sizeof(bad_str_map_t));\
})

#define BAD_STR_MAP_GET_KEYS_END(name,map_size)({\
    _Static_assert(sizeof(*(name)) == 1,"Macro requires byte ptr");\
    (BAD_STR_MAP_GET_KEYS_PTR(name) + (sizeof(void*) * (map_size << 1)));\
})

#define BAD_STR_MAP_GET_DATA_PTR(name,map_size)({\
    _Static_assert(sizeof(*(name)) == 1,"Macro requires byte ptr");\
    (BAD_STR_MAP_GET_KEYS_END(name,map_size));\
})

#define BAD_STR_MAP_GET_DATA_END(name,map_size)({\
    _Static_assert(sizeof(*(name)) == 1,"Macro requires byte ptr");\
    (BAD_STR_MAP_GET_KEYS_END(name,map_size) + (sizeof(void*) * (map_size << 1));\
})

#define BAD_STR_MAP_GET_ARENA_PTR(name,map_size)({\
    _Static_assert(sizeof(*(name)) == 1,"Macro requires byte ptr");\
    (BAD_STR_MAP_GET_KEYS_END(name,map_size));\
})

#define BAD_STR_MAP_ALLOCATE_STATIC(name,map_size,map_arena_size)\
    uint8_t BAD_STR_MAP_ALIGN_ATTR name##_map_mem[BAD_STR_MAP_ALLOC_SIZE(map_size,map_arena_size)];\
    bad_str_map_t *name = bad_str_map_init(name##_map_mem,\
        BAD_STR_MAP_GET_KEYS_PTR(name##_map_mem),\
        BAD_STR_MAP_GET_DATA_PTR(name##_map_mem,map_size),\
        BAD_STR_MAP_GET_ARENA_PTR(name##_map_mem,map_size),\
        (map_size),(map_arena_size))

BAD_STR_MAP_DEF bad_str_map_t *bad_str_map_init(
        void *str_map_mem,
        void *keys_mem,
        void *data_mem,
        void *arena_mem, 
        uint32_t map_size, 
        uint32_t arena_size);
BAD_STR_MAP_DEF void *bad_str_map_lookup_cstr_convert(bad_str_map_t *map,char *key,fstring_t *buffer);
BAD_STR_MAP_DEF void *bad_str_map_lookup_fstr(bad_str_map_t* map, fstring_t *key);
BAD_STR_MAP_DEF void *bad_str_map_lookup_cstr(bad_str_map_t *map,char* key);
BAD_STR_MAP_DEF bad_str_map_status_t bad_str_map_add_fstr(bad_str_map_t* map,fstring_t* key,void* val);
BAD_STR_MAP_DEF bad_str_map_status_t bad_str_map_add_cstr(bad_str_map_t *map,char *key,void *val);
BAD_STR_MAP_DEF void bad_str_map_reset(bad_str_map_t *map);
BAD_STR_MAP_DEF void bad_str_map_deinit(bad_str_map_t *map);
#ifdef BAD_STR_MAP_IMPLEMENTATION

bad_str_map_t *bad_str_map_init(
        void *str_map_mem,
        void *keys_mem,
        void *data_mem,
        void *arena_mem, 
        uint32_t map_size, 
        uint32_t arena_size)
{
    bad_str_map_t *map = (bad_str_map_t *)str_map_mem;
    map->map_size = map_size << 1;
    map->arena = arena_mem;
    map->arena_size = arena_size;
    map->count = 0;
    map->curr = arena_mem;
    map->data = data_mem;
    map->keys = keys_mem;
#ifdef BAD_STR_MAP_USE_STRING_H
    memset(keys_mem,0,map_size * sizeof(fstring_t **));
    memset(data_mem,0,map_size * sizeof(void **))
#else
    void **data_ptr = data_mem;
    for(uint32_t i = 0; i < map_size << 1; i++){
        data_ptr[i] = 0;
    }
    fstring_t **keys_ptr = keys_mem;
    for(uint32_t i = 0; i < map_size << 1; i++){
        keys_ptr[i] = 0;
    }
#endif
    return map;
}

void bad_str_map_reset(bad_str_map_t *map){
    map->count = 0;
    map->curr = map->arena;
#ifdef BAD_STR_MAP_USE_STRING_H
    memset(map->keys,0,map_size * sizeof(fstring_t **));
    memset(map->data,0,map_size * sizeof(void **))
#else
    void **data_ptr = map->data;
    uint32_t map_size = map->map_size;
    for(uint32_t i = 0; i < map_size; i++){
        data_ptr[i] = 0;
    }
    fstring_t **keys_ptr = map->keys;
    for(uint32_t i = 0; i < map_size; i++){
        keys_ptr[i] = 0;
    }
#endif

}

void bad_str_map_deinit(bad_str_map_t *map){
    *map = (bad_str_map_t){0};
}

static inline uint32_t bad_str_map_get_crc();
static inline void bad_str_map_update_crc(uint32_t val);
static inline void bad_str_map_reset_crc(); 

static inline uint32_t fstr_compare(fstring_t *str0,fstring_t * str1){
    if(str0->length != str1->length ){
        return 0;
    }

#ifdef BAD_STR_MAP_USE_MEMCMP
    return !memcmp(str0->data, str1->data, str1->length);
#else 
    uint32_t *wstr0 = (uint32_t*)str0->data;
    uint32_t *wstr1 = (uint32_t*)str1->data;
    uint32_t iter_count = (str0->length >> 2) + 1 ;
    for(uint32_t i = 0; i < iter_count;i++){
       if(wstr1[i]!=wstr0[i]){
           return 0;
       }
    }
    return 1;
#endif
}

static inline uint32_t fstr_cstr_compare(fstring_t *str0,char* str1,uint32_t str1_length){
    if(str0->length != str1_length ){
        return 0;
    }


#ifdef BAD_STR_MAP_USE_STRING_H
    return !memcmp(str0->data, str1, str1_length);
#else 
    uint32_t *wstr0 = (uint32_t*)str0->data;
    uint32_t *wstr1 = (uint32_t*)str1;
    uint32_t word_count = (str1_length >> 2);
    uint32_t byte_count = str1_length & 0x3;
    for(uint32_t i = 0; i < word_count;i++){
        if(wstr1[i]!=wstr0[i]){
           return 0;
        }
    }
    char* byte_str0 = (char *)(wstr0+word_count);
    char* byte_str1 = (char *)(wstr1+word_count);
    switch (byte_count) {
        case 3:{
             if(byte_str0[2] != byte_str1[2]){
                 return 0;
             }
        } 
        /*fallthrough*/
        case 2:{
            if(byte_str0[1] != byte_str1[1]){
                 return 0;
            }
        } 
        /*fallthrough*/
        case 1:{
            if(byte_str0[0] != byte_str1[0]){
                 return 0;
            }
        }
    }
    return 1;
#endif
}

static inline void *bad_str_map_arena_alloc(bad_str_map_t *map,uint32_t size,uint32_t alignment){
        
    if((alignment-1) & alignment || alignment == 0 ){
        //add scream at the programmer here
        return 0;
    }
    uint32_t padding = (alignment - ((uintptr_t)map->curr & (alignment - 1)))&(alignment - 1);
    uint32_t offset = map->curr+size +padding - map->arena; 
    if(offset > map->arena_size){
        //add scream at the programmer here 
        return 0;
    }
    
    void* result  = map->curr + padding;
    
    map->curr += padding+size;
    return result;
}

static inline bad_str_map_status_t bad_str_map_arena_append_word(bad_str_map_t* map,uint32_t word){
    uint32_t offset = (map->curr+sizeof(word) - map->arena); 
    if(offset > map->arena_size){
        //add scream at the programmer here 
        return BAD_STR_MAP_ALLOC_FAIL;
    }
    uint32_t *word_ptr  = (uint32_t*)(map->curr);
    *word_ptr = word;
    map->curr += sizeof(word) ;
    return BAD_STR_MAP_OK;
}

static inline void bad_str_map_arena_rollback(bad_str_map_t *map,void* point){
    
    void *handyptr = (map->arena +map->arena_size);
    if(point > handyptr || point < (void *)map->arena){
        //add scream at the programmer here
        return;
    }
    map->curr = (uint8_t *) point;
}

static inline void* bad_str_map_arena_fstr_start(bad_str_map_t *map,uint32_t alignment){
    
    uint32_t padding = (alignment - ((uintptr_t)map->curr & (alignment - 1)))&(alignment - 1);
    uint32_t offset = map->curr+padding - map->arena; 
    if(offset > map->arena_size){
        //add scream at the programmer here 
        return 0;
    }
    return  map->curr+= padding;
}

void* bad_str_map_lookup_fstr(bad_str_map_t* map, fstring_t *key){
    uint32_t iter_count = (key->length >> 2) + 1;
    bad_str_map_reset_crc();
    uint32_t *word_ptr = (uint32_t*)key->data;
    for(uint32_t i = 0;i < iter_count;i++){
       bad_str_map_update_crc(word_ptr[i]);
    }
    
    uint32_t idx = bad_str_map_get_crc() & (map->map_size-1);

    while(map->keys[idx]){
        if(fstr_compare(key, map->keys[idx])){
            return map->data[idx];
        }
        idx = (idx + 1) & (map->map_size-1);
    }
    
    return 0;
}

void* bad_str_map_lookup_cstr_convert(bad_str_map_t *map,char *key,fstring_t *buffer){
    uint32_t *src = (uint32_t*)key;
    uint32_t *dest = (uint32_t*)buffer->data;
    
    uint32_t word = *src++;
    
    uint32_t zero_check;    
    bad_str_map_reset_crc(); 
    uint32_t length = 0;
    
    while(!(zero_check = (word - 0x01010101) & (~(word) & 0x80808080))){
        bad_str_map_update_crc(word);
        length+=4;
        *dest++ = word;
        word = *src++;
    }
    
    uint32_t mask = (1U << __builtin_ctz(zero_check)) - 1; 
    
    length += ((__builtin_ctz(zero_check)) >> 3);
    word &= mask;
    *dest++ = word;
    buffer->length = length;
    bad_str_map_update_crc(word);    
    uint32_t idx = bad_str_map_get_crc() & (map->map_size - 1);
    
    while(map->keys[idx]){
        if(fstr_compare(buffer, map->keys[idx])){
            return map->data[idx];
        }
        idx = (idx + 1) & (map->map_size-1);
    }

    return 0; 
}

void* bad_str_map_lookup_cstr(bad_str_map_t *map,char* key){
    uint32_t *word_ptr = (uint32_t*)key;
    uint32_t word = *word_ptr++;
    
    uint32_t zero_check;    
    bad_str_map_reset_crc();
    uint32_t length = 0;
    
    while(!(zero_check = (word - 0x01010101) & (~(word) & 0x80808080))){
        bad_str_map_update_crc(word);
        length+=4;
        word = *word_ptr++;
    }
    uint32_t mask = (1U << __builtin_ctz(zero_check)) - 1; 
    
    length += ((__builtin_ctz(zero_check)) >> 3);
    word &= mask;
    bad_str_map_update_crc(word);
    uint32_t idx = bad_str_map_get_crc() & (map->map_size - 1);
    
    while(map->keys[idx]){
        if(fstr_cstr_compare(map->keys[idx],key,length)){
            return map->data[idx];
        }
        idx = (idx+1) & (map->map_size-1);
    }
    
    return 0;
}

bad_str_map_status_t bad_str_map_add_cstr(bad_str_map_t *map,char *key,void *val){
    if(map->count >= map->map_size>>1){
        return BAD_STR_MAP_FULL;
    }

    bad_str_map_reset_crc();
    uint32_t length = 0;
    fstring_t *new_str = bad_str_map_arena_alloc(map,sizeof(fstring_t),sizeof(uint32_t));
    if(!new_str){
        return BAD_STR_MAP_ALLOC_FAIL;
    }
    new_str->data = bad_str_map_arena_fstr_start(map,sizeof(uint32_t));
    
    if(!new_str->data){
        return BAD_STR_MAP_ALLOC_FAIL;
    }
    
    uint32_t *word_ptr = (uint32_t*) key;
    uint32_t word = *word_ptr++;
    
    uint32_t zero_check;    
    
    while(!(zero_check = (word - 0x01010101) & (~(word) & 0x80808080))){
        if(bad_str_map_arena_append_word(map,word) != BAD_STR_MAP_OK){
            bad_str_map_arena_rollback(map,new_str);
            return BAD_STR_MAP_ALLOC_FAIL;
        }
        bad_str_map_update_crc(word);
        length+=4;
        word = *word_ptr++;
    }

    uint32_t mask = (1U << __builtin_ctz(zero_check)) - 1; 
    
    length += ((__builtin_ctz(zero_check)) >> 3);
    word &= mask;
    if(bad_str_map_arena_append_word(map,word) != BAD_STR_MAP_OK){
        bad_str_map_arena_rollback(map,new_str);
        return BAD_STR_MAP_ALLOC_FAIL;
    }
    new_str->length = length;
    bad_str_map_update_crc(word);
    uint32_t idx = bad_str_map_get_crc() & (map->map_size - 1);

    while(map->keys[idx]){
        if(fstr_compare(new_str, map->keys[idx])){
            bad_str_map_arena_rollback(map,new_str);
            return BAD_STR_MAP_ALREADY_EXISTS;
        }
        idx = (idx + 1) & (map->map_size-1);
    }
    map->keys[idx] = new_str;
    map->data[idx] = val;
    map->count++;
    return BAD_STR_MAP_OK;
}

bad_str_map_status_t bad_str_map_add_fstr(bad_str_map_t* map,fstring_t* key,void* val){
    if(map->count >= map->map_size>>1){
        return BAD_STR_MAP_FULL;
    }

    uint32_t iter_count = (key->length >> 2) + 1;
    bad_str_map_reset_crc();
    uint32_t *word_ptr = (uint32_t*)key->data;
    fstring_t *new_str = bad_str_map_arena_alloc(map,sizeof(fstring_t),sizeof(uint32_t));

    if(!new_str){
        return BAD_STR_MAP_ALLOC_FAIL;
    }

    new_str->data = bad_str_map_arena_fstr_start(map,sizeof(uint32_t));
    
    if(!new_str->data){
        return BAD_STR_MAP_ALLOC_FAIL;
    }


    for(uint32_t i = 0;i < iter_count;i++){
        bad_str_map_update_crc(word_ptr[i]);
        if(bad_str_map_arena_append_word(map,word_ptr[i]) != BAD_STR_MAP_OK){
            bad_str_map_arena_rollback(map,new_str);
            return BAD_STR_MAP_ALLOC_FAIL;
        }
    }

    uint32_t idx = bad_str_map_get_crc() & (map->map_size - 1);
    while(map->keys[idx]){
        if(fstr_compare(key, map->keys[idx])){
            bad_str_map_arena_rollback(map, new_str);
            return BAD_STR_MAP_ALREADY_EXISTS;
        }
        idx = (idx + 1) & (map->map_size-1);
    }
    map->keys[idx] = new_str;
    map->data[idx] = val;
    map->count++;
    return BAD_STR_MAP_OK;

}
#endif
#endif
