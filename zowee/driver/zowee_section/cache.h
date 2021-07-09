#ifndef __CACHE_H
#define __CACHE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "string.h"
#include "plat_types.h"
#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CACHE   20

#ifndef ssize_t
typedef int                         ssize_t;
#endif

typedef struct
{
    uint8_t *buffer;
    size_t size;
    __IO uint16_t write_pointer;
    __IO uint16_t read_pointer;
} loop_cache_t;

typedef struct
{
    int id;
    loop_cache_t cache;
} loop_cache_id_t;

bool get_4k_cache_pool_used_flag(void);
int get_4k_cache_pool(uint8_t **cache);
void _4k_cache_pool_free(void);
void loop_cache_init(void);
int loop_cache_register(uint8_t *buffer, uint16_t size, uint16_t write_pointer, uint16_t read_pointer, uint8_t clear_flag);
int write_loop_cache(int id, const uint8_t *buffer, int cnt, uint8_t overflow_cover_flag);
ssize_t read_loop_cache(int id, uint8_t *buffer, uint16_t cnt);
ssize_t read_loop_cache_buffer_cnt(int id);
ssize_t read_loop_cache_buffer_remain(int id);
int loop_cache_buffer_full(int id);
int loop_cache_buffer_empty(int id);
int loop_cache_buffer_clear(int id);
int loop_cache_sort_out(int id, uint16_t pointer);
int get_loop_cache_param(int id, uint16_t *write_pointer, uint16_t *read_pointer);
int loop_cache_param_config(int id, uint16_t write_pointer, uint16_t read_pointer);

#ifdef __cplusplus
}
#endif

#endif

