#include <stdint.h>
#ifdef BAD_STRMAP_USE_MEMCMP
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

#define BAD_STR_MAP_CREATE_STATIC(name,size,map_arena_size)                 \
    _Static_assert((size & (size-1)) == 0,"Map size must be a power of 2"); \
    _Static_assert((map_arena_size & 0x3) == 0,"Map arena size must be a multiple of word_size" );\
    fstring_t *name##_keys[size << 1] = {0};                                \
    void * name##_data[size << 1];                                          \
    uint8_t __attribute__((aligned(4)))name##_arena[map_arena_size];        \
    bad_str_map_t name = {.arena =name##_arena,                             \
        .data = name##_data,                                                \
        .curr = name##_arena,                                               \
        .keys = name##_keys,                                                \
        .map_size = size<<1,                                                \
        .arena_size = map_arena_size};
#ifdef BAD_STRMAP_STATIC
    #define BAD_STRMAP_DEF static
#else 
    #define BAD_STRMAP_DEF extern
#endif // BAD_STRMAP_STATIC

BAD_STRMAP_DEF void* bad_str_map_lookup_cstr_convert(bad_str_map_t *map,char *key,fstring_t *buffer);
BAD_STRMAP_DEF void *bad_str_map_lookup_fstr(bad_str_map_t* map, fstring_t *key);
BAD_STRMAP_DEF void* bad_str_map_lookup_cstr(bad_str_map_t *map,char* key);
BAD_STRMAP_DEF bad_str_map_status_t bad_str_map_add_fstr(bad_str_map_t* map,fstring_t* key,void* val);
BAD_STRMAP_DEF bad_str_map_status_t bad_str_map_add_cstr(bad_str_map_t *map,char *key,void *val);

#ifdef BAD_STRMAP_IMPLEMENTATION

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
    uint32_t diff = 0;
    for(uint32_t i = 0; i < iter_count;i++){
       diff |= wstr1[i] ^ wstr0[i];
    }
    return !diff;
#endif
}

static inline uint32_t fstr_cstr_compare(fstring_t *str0,char* str1,uint32_t str1_length){
    if(str0->length != str1_length ){
        return 0;
    }


#ifdef BAD_STR_MAP_USE_MEMCMP
    return !memcmp(str0->data, str1, str1_length);
#else 
    uint32_t *wstr0 = (uint32_t*)str0->data;
    uint32_t *wstr1 = (uint32_t*)str1;
    uint32_t word_count = (str1_length >> 2);
    uint32_t byte_count = str1_length & 0x3;
    uint32_t diff = 0;
    for(uint32_t i = 0; i < word_count;i++){
        diff |= (wstr1[i] ^ wstr0[i]);
    }
    char* byte_str0 = (char *)(wstr0+word_count);
    char* byte_str1 = (char *)(wstr1+word_count);
    switch (byte_count) {
        case 3:{
             diff |= (byte_str1[2] ^ byte_str0[2]);
        } 
        /*fallthrough*/
        case 2:{
            diff |= (byte_str1[1] ^ byte_str0[1]);
        } 
        /*fallthrough*/
        case 1:{
            diff |= (byte_str1[0] ^ byte_str0[0]);
        }
    }
    return !diff;
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
