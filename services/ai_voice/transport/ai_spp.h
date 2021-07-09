#ifndef __AI_SPP_H__
#define __AI_SPP_H__

#include "app_spp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_AI_SPP_ENTRY_INDEX 0xFFFF

#define SPP1 0
#define SPP2 1
#define SPP_NUM 2

extern uint32_t __ai_spp_register_table_start[];
extern uint32_t __ai_spp_register_table_end[];

typedef void (*ai_spp_register_128uuid_sdp_services)(uint8_t *service_id, btif_sdp_record_param_t *param);

/**
 * @brief AI voice spp_init definition data structure
 *
 */
typedef struct
{
    uint32_t                                ai_code;
    ai_spp_register_128uuid_sdp_services    ai_handler;     /**< ai spp register handler */
} AI_SPP__REGISTER_INSTANCE_T;


#define AI_SPP_TO_ADD(ai_code, ai_handler)  \
    static const AI_SPP__REGISTER_INSTANCE_T ai_code##_spp_register __attribute__((used, section(".ai_spp_register_table"))) =    \
        {(ai_code), (ai_handler)};

#define AI_SPP_PTR_FROM_ENTRY_INDEX(index)  \
    ((AI_SPP__REGISTER_INSTANCE_T *)((uint32_t)__ai_spp_register_table_start + (index)*sizeof(AI_SPP__REGISTER_INSTANCE_T)))

void app_ai_spp_server_init(void);
void app_ai_link_free_after_spp_dis(void);
bool app_ai_spp_send(uint8_t* ptrData, uint32_t length);
bool app_ai_spp_is_connected(void);
void *app_ai_spp_get_remdev_btaddr(void);
void app_ai_spp_disconnlink(void); //only used for device force disconnect
void app_ai_spp_deinit_tx_buf(void);
void ai_spp_register_uuid_sdp_services(struct _spp_dev *dev);

#ifdef __cplusplus
}
#endif


#endif
