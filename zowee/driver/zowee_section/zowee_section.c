#if defined(__APP_ZOWEE_UI__)
#include "cmsis.h"
#include "plat_types.h"
#include "tgt_hardware.h"
#include "string.h"
#include "export_fn_rom.h"
#include "zowee_section.h"
#include "pmu.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#include "norflash_api.h"
#include "heap_api.h"
#include "cache.h"

#define crc32(crc, buf, len) __export_fn_rom.crc32(crc, buf, len)

extern uint32_t __zowee_parameter_start[];
extern uint32_t __zowee_parameter_end[];
static zowee_section_t *zowee_section_p = NULL;

static void zowee_section_callback(void* param)
{
    NORFLASH_API_OPERA_RESULT *opera_result;

    opera_result = (NORFLASH_API_OPERA_RESULT*)param;

    ZOWEE_SECTION_TRACE(": type = %d, addr = 0x%x,len = 0x%x,result = %d.", 
                opera_result->type,
                opera_result->addr,
                opera_result->len,
                opera_result->result);
}

void zowee_section_init(void)
{
    enum NORFLASH_API_RET_T result;
    uint32_t sector_size = 0;
    uint32_t block_size = 0;
    uint32_t page_size = 0;

    hal_norflash_get_size(HAL_NORFLASH_ID_0,
               NULL,
               &block_size,
               &sector_size,
               &page_size);
    result = norflash_api_register(NORFLASH_API_MODULE_ID_ZOWEE_DATA,
                    HAL_NORFLASH_ID_0,
                    ((uint32_t)__zowee_parameter_start)&0x00FFFFFF,
                    (uint32_t)__zowee_parameter_end - (uint32_t)__zowee_parameter_start,
                    block_size,
                    sector_size,
                    page_size,
                    ZOWEE_SECTOR_SIZE,
                    zowee_section_callback
                    );
    ASSERT(result == NORFLASH_API_OK,"zowee_section_init: module register failed! result = %d.",result);
}

int zowee_section_open(void)
{
    uint32_t lock;
    uint8_t *mempool = NULL;
    enum NORFLASH_API_RET_T ret;

    zowee_section_p = (zowee_section_t *)__zowee_parameter_start;

	ZOWEE_SECTION_TRACE("head.magic: 0x%x, head.version: 0x%x !!!", zowee_section_p->head.magic, zowee_section_p->head.version);
	if (zowee_section_p->head.magic != nvrec_zowee_dev_magic) {
		ZOWEE_SECTION_TRACE("zowee section is invalid, need restore");
		
		if(get_4k_cache_pool_used_flag() == true)
		{
			ZOWEE_SECTION_TRACE("cache bufffer is busy!");
			return -2;
		}
		get_4k_cache_pool((uint8_t **)&mempool);
		
		memcpy(mempool, zowee_section_p, 0x1000);

        //syspool_init();
        //syspool_get_buff((uint8_t **)&mempool, 0x1000);
        //memset(mempool, 0x00, 0x1000);		
        ((zowee_section_t *)mempool)->head.magic = nvrec_zowee_dev_magic;
        ((zowee_section_t *)mempool)->head.version = nvrec_zowee_current_version;	
		//ZOWEE_SECTION_TRACE("p:%p size:%d",(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),
			//sizeof(zowee_section_t)-2-2-4);	

        ((zowee_section_t *)mempool)->head.crc = crc32(0,(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),
			sizeof(zowee_section_t)-2-2-4);
		

        lock = int_lock_global();
        ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
        ASSERT(ret == NORFLASH_API_OK,"zowee_section_open: erase failed! ret = %d.",ret);
        ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
        ASSERT(ret == NORFLASH_API_OK,"zowee_section_open: write failed! ret = %d.",ret);
        int_unlock_global(lock);

		_4k_cache_pool_free();
    }    
    return 0;
}
#if 0
int zowee_section_get_touch_threshold_value()
{
	uint8_t threshold;
	
	if (zowee_section_p)	
	{
		threshold = zowee_section_p->data.touch_threshold; 
		threshold = threshold*800/256;	//delta = threshold*ATI target/256
		ZOWEE_SECTION_TRACE("zowee_section_get_touch_threshold_value :%d",threshold);
		
		return threshold;
	} 
	else 
	{
		return -1;
	}
}

int zowee_section_set_touch_threshold_value(uint8_t threshold)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if (zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("zowee_section_set_touch_threshold_value");
    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("threshold:%d", threshold);
	((zowee_section_t *)mempool)->data.touch_threshold = threshold;
    
    ((zowee_section_t *)mempool)->head.crc = \
        crc32(0,(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),sizeof(zowee_section_t)-2-2-4);

    ZOWEE_SECTION_TRACE("psfree:%d", ((zowee_section_t *)mempool)->data.touch_threshold);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_touch_threshold_value: erase failed! ret = %d.",ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_touch_threshold_value: write failed! ret = %d.",ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}
#endif
int zowee_section_get_earside(void)
{
	uint8_t earside;
	
	if (zowee_section_p)	
	{
		earside = zowee_section_p->data.earside; 
		ZOWEE_SECTION_TRACE("zowee_section_get_earside :%d",earside);
		
		return earside;
	} 
	else 
	{
		return -1;
	}
}

int zowee_section_set_earside(unsigned int earside)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if (zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("zowee_section_set_earside");
    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("earside:%d", earside);
	((zowee_section_t *)mempool)->data.earside = earside;
    
    ((zowee_section_t *)mempool)->head.crc = crc32(0,
    			(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),sizeof(zowee_section_t)-2-2-4);

    //ZOWEE_SECTION_TRACE("psfree:%d", ((zowee_section_t *)mempool)->data.earside);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: erase failed! ret = %d.",ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: write failed! ret = %d.",ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}

#if defined(__IR_W2001__)
int zowee_section_get_ir_calib_value(nvrec_pt_dev_ir_calib_type IRdistanceGrad, uint32_t *buffer)
{
	if (zowee_section_p)	{
        switch(IRdistanceGrad)
        {
            case PT_IR_CT:
                if (zowee_section_p->data.ir_calib_status & (0x01<<PT_IR_CT)) {
                    *buffer = zowee_section_p->data.ir_calib_value.ps_ct;
                } else {
                    return -2;
                }
                break;
                
            case PT_IR_5MM:
                if (zowee_section_p->data.ir_calib_status & (0x01<<PT_IR_5MM)) {
                    *buffer = zowee_section_p->data.ir_calib_value.ps_5mm;
                } else {
                    return -2;
                }
                break;
                
            case PT_IR_NULL:
                if (zowee_section_p->data.ir_calib_status & (0x01<<PT_IR_NULL)) {
                    *buffer = zowee_section_p->data.ir_calib_value.ps_free;
                } else {
                    return -2;
                }
                break;
                
            default:
                break;
        }      
	} else {
		return -1;
	}

	return 0;
}

int zowee_section_set_ir_calib_value(nvrec_pt_dev_ir_calib_type IRdistanceGrad, uint32_t buffer)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if (zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("zowee_section_set_ir_calib_value");
    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("IRdistanceGrad = %x. value:%d", IRdistanceGrad, buffer);
    switch(IRdistanceGrad)
    {
        case PT_IR_CT:
            ((zowee_section_t *)mempool)->data.ir_calib_value.ps_ct = buffer;
            ((zowee_section_t *)mempool)->data.ir_calib_status |= (0x01<<PT_IR_CT);
            break;
        case PT_IR_5MM:
            ((zowee_section_t *)mempool)->data.ir_calib_value.ps_5mm = buffer;
            ((zowee_section_t *)mempool)->data.ir_calib_status |= (0x01<<PT_IR_5MM);
            break;
        case PT_IR_NULL:
            ((zowee_section_t *)mempool)->data.ir_calib_value.ps_free = buffer;
            ((zowee_section_t *)mempool)->data.ir_calib_status |= (0x01<<PT_IR_NULL);
            break;
            
        default:
            break;
    }
    
    ((zowee_section_t *)mempool)->head.crc = \
        crc32(0,(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),sizeof(zowee_section_t)-2-2-4);

    ZOWEE_SECTION_TRACE("ps_ct:%d, ps_5mm:%d, ps_free:%d", 
        ((zowee_section_t *)mempool)->data.ir_calib_value.ps_ct,
        ((zowee_section_t *)mempool)->data.ir_calib_value.ps_5mm,
        ((zowee_section_t *)mempool)->data.ir_calib_value.ps_free);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_ir_calib_value: erase failed! ret = %d.",ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_ir_calib_value: write failed! ret = %d.",ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}
#endif


int zowee_section_get_box_state(void)
{
	uint8_t box_state;
	
	if (zowee_section_p)	
	{
		box_state = zowee_section_p->data.box_state; 
		ZOWEE_SECTION_TRACE(": %d",box_state);
		
		return box_state;
	} 
	else 
	{
		return -1;
	}
}

int zowee_section_set_box_state(unsigned int box_state)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if (zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("!!!");
	
    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("box_state:%d", box_state);
	((zowee_section_t *)mempool)->data.box_state = box_state;
    
    ((zowee_section_t *)mempool)->head.crc = crc32(0,
    			(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),sizeof(zowee_section_t)-2-2-4);

    //ZOWEE_SECTION_TRACE("psfree:%d", ((zowee_section_t *)mempool)->data.earside);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: erase failed! ret = %d.",ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: write failed! ret = %d.",ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}


int zowee_section_get_anc_state(void)
{
	uint8_t anc_state;
	
	if (zowee_section_p)	
	{
		anc_state = zowee_section_p->data.anc_state; 
		ZOWEE_SECTION_TRACE(": %d",anc_state);
		
		return anc_state;
	} 
	else 
	{
		return -1;
	}
}

int zowee_section_set_anc_state(unsigned int anc_state)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if (zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("!!!");
	
    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("anc_state:%d", anc_state);
	((zowee_section_t *)mempool)->data.anc_state = anc_state;
    
    ((zowee_section_t *)mempool)->head.crc = crc32(0,
    			(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)),sizeof(zowee_section_t)-2-2-4);

    //ZOWEE_SECTION_TRACE("psfree:%d", ((zowee_section_t *)mempool)->data.earside);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: erase failed! ret = %d.",ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA,((uint32_t)__zowee_parameter_start)&0x00FFFFFF,mempool,0x1000,false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: write failed! ret = %d.",ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}


int zowee_section_get_battery_level_state(void)
{
	uint8_t battery_level_state;
	
	if(zowee_section_p)	
	{
		battery_level_state = zowee_section_p->data.battery_level_state; 
		ZOWEE_SECTION_TRACE(": %d", battery_level_state);
		
		return battery_level_state;
	} 
	else 
	{
		return -1;
	}
}

int zowee_section_set_battery_level_state(unsigned int battery_level_state)
{
    uint8_t *mempool = NULL;
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    if(zowee_section_p == NULL)
    {
        return -1;
    }

    ZOWEE_SECTION_TRACE("!!!");

    if(get_4k_cache_pool_used_flag() == true)
    {
        ZOWEE_SECTION_TRACE("cache bufffer is busy!");
        return -2;
    }
    get_4k_cache_pool((uint8_t **)&mempool);

    memcpy(mempool, zowee_section_p, 0x1000);

    ZOWEE_SECTION_TRACE("battery_level_state : %d", battery_level_state);
	
	((zowee_section_t *)mempool)->data.battery_level_state = battery_level_state;
    
    ((zowee_section_t *)mempool)->head.crc = crc32(0, 
    			(unsigned char *)(&(((zowee_section_t *)mempool)->head.reserved0)), sizeof(zowee_section_t)-2-2-4);

    //ZOWEE_SECTION_TRACE("psfree:%d", ((zowee_section_t *)mempool)->data.earside);

    lock = int_lock_global();
    ret = norflash_api_erase(NORFLASH_API_MODULE_ID_ZOWEE_DATA, ((uint32_t)__zowee_parameter_start)&0x00FFFFFF, 0x1000, false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: erase failed! ret = %d.", ret);
    ret = norflash_api_write(NORFLASH_API_MODULE_ID_ZOWEE_DATA, ((uint32_t)__zowee_parameter_start)&0x00FFFFFF, mempool, 0x1000, false);
    ASSERT(ret == NORFLASH_API_OK,"zowee_section_set_earside: write failed! ret = %d.", ret);
    int_unlock_global(lock);

	_4k_cache_pool_free();

    return 0;
}



#endif // #if defined(__APP_ZOWEE_UI__)

