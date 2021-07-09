#include "cache.h"

static loop_cache_t *loop_cache;
static loop_cache_id_t loop_cache_id[MAX_CACHE];
static int loop_cache_id_cnt = 0;
static uint8_t _4k_cache_pool[0x1000];
static bool _4k_cache_pool_used_flag = false;

bool get_4k_cache_pool_used_flag(void)
{
    return _4k_cache_pool_used_flag;
}

int get_4k_cache_pool(uint8_t **cache)
{
    if(_4k_cache_pool_used_flag == true)
    {
        return -1;
    }

    ASSERT(sizeof(_4k_cache_pool) == 0x1000, "size of cache_pool must be 0x1000!");

    _4k_cache_pool_used_flag = true;
    *cache = _4k_cache_pool;
    memset(_4k_cache_pool, 0x00, sizeof(_4k_cache_pool));

    return sizeof(_4k_cache_pool);
}

void _4k_cache_pool_free(void)
{
    _4k_cache_pool_used_flag = false;
    memset(_4k_cache_pool, 0x00, sizeof(_4k_cache_pool));
}

void loop_cache_init(void)
{
    static uint8_t init_flag = 0;

    if (init_flag == 1)
    {
        return;
    }

    memset(loop_cache_id, 0x00, sizeof(loop_cache_id));
    loop_cache_id_cnt = 0;
}

int loop_cache_register(uint8_t *buffer, uint16_t size, uint16_t write_pointer, uint16_t read_pointer, uint8_t clear_flag)
{
    int current_loop_cache_id = 0;

    if (loop_cache_id_cnt > MAX_CACHE)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[loop_cache_id_cnt].cache;

    loop_cache->buffer = buffer;
    loop_cache->size = size;
    loop_cache->write_pointer = write_pointer;
    loop_cache->read_pointer = read_pointer;

    if (clear_flag == 1)
    {
        memset(loop_cache->buffer, 0x00, loop_cache->size);
    }

    current_loop_cache_id = loop_cache_id_cnt;
    loop_cache_id_cnt++;

    return current_loop_cache_id;
}

int write_loop_cache(int id, const uint8_t *buffer, int cnt, uint8_t overflow_cover_flag)
{
    int temp_cnt = 0;

    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    if ( (read_loop_cache_buffer_remain(id) < cnt) &&
         (overflow_cover_flag == 0))
    {
        temp_cnt = read_loop_cache_buffer_remain(id);
    }
    else
    {
        temp_cnt = cnt;
    }

    for (uint16_t i = 0; i < temp_cnt; i++)
    {
        loop_cache->buffer[loop_cache->write_pointer++] = buffer[i];

        if (loop_cache->write_pointer >= loop_cache->size)
        {
            loop_cache->write_pointer = 0;
        }
    }

    return temp_cnt;
}

ssize_t read_loop_cache(int id, uint8_t *buffer, uint16_t cnt)
{
    uint16_t get_cnt = 0;

    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    if (loop_cache->write_pointer == loop_cache->read_pointer)
    {
        return 0;
    }

    if (loop_cache->write_pointer > loop_cache->read_pointer)
    {
        if (cnt <= loop_cache->write_pointer - loop_cache->read_pointer)
        {
            get_cnt = cnt;
        }
        else
        {
            get_cnt = loop_cache->write_pointer - loop_cache->read_pointer;
        }
    }
    else
    {
        if (cnt <= loop_cache->size - loop_cache->read_pointer + loop_cache->write_pointer)
        {
            get_cnt = cnt;
        }
        else
        {
            get_cnt = loop_cache->size - loop_cache->read_pointer + loop_cache->write_pointer;
        }
    }

    for (uint16_t i = 0; i < get_cnt; i++)
    {
        buffer[i] = loop_cache->buffer[loop_cache->read_pointer++];

        if (loop_cache->read_pointer >= loop_cache->size)
        {
            loop_cache->read_pointer = 0;
        }
    }

#if 0
    TRACE2("\r\nloop_cache->read_pointer = %d.", loop_cache->read_pointer);
    TRACE2("\r\nloop_cache->write_pointer = %d.", loop_cache->write_pointer);
    TRACE2("\r\nget_cnt = %d.", get_cnt);
#endif

    return get_cnt;
}

ssize_t read_loop_cache_buffer_cnt(int id)
{
    ssize_t buffer_data_cnt = 0;

    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    if (loop_cache->write_pointer >= loop_cache->read_pointer)
    {
        buffer_data_cnt = loop_cache->write_pointer - loop_cache->read_pointer;
    }
    else
    {
        buffer_data_cnt = loop_cache->size - loop_cache->read_pointer + loop_cache->write_pointer;
    }

    return buffer_data_cnt;
}

ssize_t read_loop_cache_buffer_remain(int id)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    return (loop_cache->size - read_loop_cache_buffer_cnt(id));
}

int loop_cache_buffer_full(int id)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    if (loop_cache->write_pointer >= loop_cache->read_pointer)
    {
        return 1;
    }
    else
    {
        if ((loop_cache->write_pointer == loop_cache->size - 1) && \
            (loop_cache->read_pointer == 0))
        {
            return 0;
        }

        if (loop_cache->write_pointer + 1 == loop_cache->read_pointer)
        {
            return 0;
        }
    }

    return 0;
}

int loop_cache_buffer_empty(int id)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    return ((loop_cache->write_pointer == \
             loop_cache->read_pointer) ? 0 : 1);
}

int loop_cache_buffer_clear(int id)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    loop_cache->write_pointer = 0;
    loop_cache->read_pointer = 0;
    memset(loop_cache->buffer, 0x00, loop_cache->size);

    return 0;
}

int loop_cache_sort_out(int id, uint16_t pointer)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    for (uint16_t i = 0; pointer + i < read_loop_cache_buffer_cnt(id); i++)
    {
        loop_cache->buffer[i] = loop_cache->buffer[pointer + i];
    }

    loop_cache->write_pointer = read_loop_cache_buffer_cnt(id);
    loop_cache->read_pointer = 0;

    return 0;
}

int get_loop_cache_param(int id, uint16_t *write_pointer, uint16_t *read_pointer)
{
    if (write_pointer == NULL)
    {
        return -1;
    }

    if (read_pointer == NULL)
    {
        return -2;
    }

    if (id > loop_cache_id_cnt)
    {
        return -3;
    }

    loop_cache = &loop_cache_id[id].cache;

    *write_pointer = loop_cache->write_pointer;
    *read_pointer = loop_cache->read_pointer;

    return 0;
}

int loop_cache_param_config(int id, uint16_t write_pointer, uint16_t read_pointer)
{
    if (id > loop_cache_id_cnt)
    {
        return -1;
    }

    loop_cache = &loop_cache_id[id].cache;

    loop_cache->write_pointer = write_pointer;
    loop_cache->read_pointer = read_pointer;

    return 0;
}


