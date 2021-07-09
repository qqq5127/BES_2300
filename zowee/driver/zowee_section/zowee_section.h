#ifndef __ZOWEE_SECTIONS_H__
#define __ZOWEE_SECTIONS_H__

#if defined(__APP_ZOWEE_UI__)
#include "stdint.h"
#include "stdbool.h"
#include "string.h"



#define ZOWEE_SECTION_TRACE_ENABLE		1

#if (ZOWEE_SECTION_TRACE_ENABLE)
#define ZOWEE_SECTION_TRACE(s, ...)		TRACE(1, "[ZOWEE_SECTION]%s " s, __func__, ##__VA_ARGS__)
#else
#define ZOWEE_SECTION_TRACE(s, ...)
#endif




#define ALIGN4 __attribute__((aligned(4)))
#define nvrec_zowee_dev_magic      		0xfeba
#define nvrec_zowee_current_version 	0x01
#define ZOWEE_SECTOR_SIZE            	4096


#if defined(__IR_W2001__)
typedef struct
{
	uint32_t ps_ct:32;
	uint32_t ps_5mm:32;
	uint32_t ps_free:32;
} nvrec_pt_dev_ir_ps_type;

typedef enum
{
	PT_IR_CT = 0,
	PT_IR_5MM,
	PT_IR_NULL,
} nvrec_pt_dev_ir_calib_type;
#endif

typedef struct{
    unsigned short magic;
    unsigned short version;
    unsigned int crc ;
    unsigned int reserved0;
    unsigned int reserved1;
} zowee_section_head_t;

typedef struct{
	//int touch_threshold;
#if defined(__IR_W2001__)
    nvrec_pt_dev_ir_ps_type ir_calib_value;
    uint32_t ir_calib_status;
#endif
	unsigned int earside;
	unsigned int box_state;
	unsigned int anc_state;
	unsigned int battery_level_state;
} zowee_section_data_t;

typedef struct{
    zowee_section_head_t head;
    zowee_section_data_t data;
} zowee_section_t;

#ifdef __cplusplus
extern "C" {
#endif

void zowee_section_init(void);
int zowee_section_open(void);

//int zowee_section_get_touch_threshold_value();
//int zowee_section_set_touch_threshold_value(int threshold);
int zowee_section_get_earside(void);
int zowee_section_set_earside(unsigned int earside);
#if defined(__IR_W2001__)
int zowee_section_get_ir_calib_value(nvrec_pt_dev_ir_calib_type IRdistanceGrad, uint32_t *buffer);
int zowee_section_set_ir_calib_value(nvrec_pt_dev_ir_calib_type IRdistanceGrad, uint32_t buffer);
#endif

int zowee_section_get_box_state(void);
int zowee_section_set_box_state(unsigned int box_state);

int zowee_section_get_anc_state(void);
int zowee_section_set_anc_state(unsigned int anc_state);

int zowee_section_get_battery_level_state(void);
int zowee_section_set_battery_level_state(unsigned int battery_level_state);



#ifdef __cplusplus
}
#endif

#endif // #if defined(__APP_ZOWEE_UI__)

#endif //__ZOWEE_SECTIONS_H__

