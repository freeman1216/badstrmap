## What is this?
A lightweight header-only owning string hashmap for mcus using CRC peripheral as a hash
## Features
- Self allocates the memory from a passed memory arena (con: you cannot remove the entires) 
- Register agnostic api
- 350 lines of C
- No external dependencies
## How to use it  
1. Include the header in your project  
2. Define "BAD_STRMAP_IMPLEMENTATION" and include the header 

```c
#define BAD_STRMAP_IMPLEMENTATION
#include "badstrmap.h"
```
3. Define the nessesary functions  
```c
static inline void bad_str_map_reset_crc(){
    crc_reset();
}

static inline uint32_t bad_str_map_get_crc(){
    return CRC->DR;
}

static inline void bad_str_map_update_crc(uint32_t val){
    CRC->DR = val;
}
```
4. Alocate the map either statically or dinamically 
```c
BAD_STR_MAP_CREATE_STATIC(map, 64, 256);

```
5. Make sure the input is 4 byte aligned
```c
char __attribute__((aligned(4))) *key ="Hello Hashmap";
```
6. Use the api
```c
bad_str_map_add_cstr(&map,key , (void *)123 );
void *res = bad_str_map_lookup_cstr(&map, key);
```
## Notes
Api casts pointers so make sure to compile it with strict aliasing disabled
The cstr api may overread so make sure the buffer is 4 byte aligned if you place it at the end of memory

The main application (`src/main.c`) provides an example

You can freely modify or copy whatever you need.
