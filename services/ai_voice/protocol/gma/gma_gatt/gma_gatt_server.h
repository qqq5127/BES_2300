#ifndef _GMA_GATT_SERVER_H_
#define _GMA_GATT_SERVER_H_

/**
 ****************************************************************************************
 * @addtogroup GMATASK Task
 * @ingroup GMA
 * @brief Gma Voice Profile Task.
 *
 * The GMATASK is responsible for handling the messages coming in and out of the
 * @ref GMA collector block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_task.h"
#include "ai_transport.h"

#define GMA_GATT_MAX_ATTR_LEN   AI_TRANS_DEINIT_MTU_SIZE

///Attributes State Machine
enum {
    GMA_IDX_SVC,

    GMA_IDX_READ_CHAR,
    GMA_IDX_READ_VAL,

    GMA_IDX_WRITE_CHAR,
    GMA_IDX_WRITE_VAL,

    GMA_IDX_INDICATE_CHAR,
    GMA_IDX_INDICATE_VAL,
    GMA_IDX_INDICATE_CFG,

    GMA_IDX_WR_NO_RSP_CHAR,
    GMA_IDX_WR_NO_RSP_VAL,

    GMA_IDX_NTF_CHAR,
    GMA_IDX_NTF_VAL,
    GMA_IDX_NTF_CFG,

    GMA_IDX_NB,
};


/// Messages for Gma Voice Profile 
enum gma_msg_id {
    GMA_CMD_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_AI),

    GMA_TX_DATA_SENT,
    
    GMA_CMD_RECEIVED,

    GMA_SEND_CMD_VIA_NOTIFICATION,

    GMA_SEND_CMD_VIA_INDICATION,

    GMA_DATA_RECEIVED,

    GMA_SEND_DATA_VIA_NOTIFICATION,

    GMA_SEND_DATA_VIA_INDICATION,
};


/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void gma_task_init(struct ke_task_desc *task_desc);

#endif //_GMA_GATT_SERVER_H_

