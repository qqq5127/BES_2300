#ifndef _TENCENT_GATT_SERVER_H_
#define _TENCENT_GATT_SERVER_H_

/**
 ****************************************************************************************
 * @addtogroup TENCENT_SMARTVOICETASK Task
 * @ingroup TENCENT_SMARTVOICE
 * @brief Smart Voice Profile Task.
 *
 * The TENCENT_SMARTVOICETASK is responsible for handling the messages coming in and out of the
 * @ref TENCENT_SMARTVOICE collector block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "ke_task.h"


///Attributes State Machine
enum {
    TENCENT_IDX_SVC,

    TENCENT_IDX_TX_CHAR,
    TENCENT_IDX_TX_VAL,
    TENCENT_IDX_TX_NTF_CFG,

    TENCENT_IDX_RX_CHAR,
    TENCENT_IDX_RX_VAL,

    TENCENT_IDX_NB,
};

/// Messages for Smart Voice Profile
enum tencent_sv_msg_id {
    TENCENT_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_AI),

    TENCENT_TX_DATA_SENT,

    TENCENT_RECEIVED,

    TENCENT_SEND_VIA_NOTIFICATION,

    TENCENT_SEND_VIA_INDICATION,

};

/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void tencent_task_init(struct ke_task_desc *task_desc);


/// @} 

#endif /* _TENCENT_SMARTVOICE_TASK_H_ */

