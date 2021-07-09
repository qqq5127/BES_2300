#ifndef __APP_SPP_ANC_H__
#define __APP_SPP_ANC_H__

#include "app_user_spp.h"

#define	SCPI_MAILBOX_MAX		10
#define	SCPI_MAILBOX_BUFF_MAX	255

typedef enum __scpi_sampling_rate_e {
	SCPI_SAMPLING_RATE_50P7K = 0,
	SCPI_SAMPLING_RATE_48K,
	SCPI_SAMPLING_RATE_44P1K,
	SCPI_SAMPLING_RATE_ERROR,
}scpi_sampling_rate_e;

typedef enum __scpi_channel_e {
	SCPI_CHANNEL_LEFT = 0,
	SCPI_CHANNEL_RIGHT,
	SCPI_CHANNEL_ERROR,
}scpi_channel_e;

typedef enum __scpi_mic_channel_e {
	SCPI_MIC_CHANNEL_FF = 0,
	SCPI_MIC_CHANNEL_FB,
	SCPI_MIC_CHANNEL_EQ,
	SCPI_MIC_CHANNEL_MC,
	SCPI_MIC_CHANNEL_ERROR,
}scpi_mic_channel_e;

typedef enum __scpi_command_index_e {
	SCPI_COMMAND_INDEX_STATE = 0,
		
}scpi_command_index_e;

typedef enum __scpi_anc_parameter_e {
	SCPI_ANC_PARAMETER_CONTROL = 0,
	SCPI_ANC_PARAMETER_TOTAL_GAIN,
	SCPI_ANC_PARAMETER_BYPASS,
	SCPI_ANC_PARAMETER_COUNT,
	SCPI_ANC_PARAMETER_ADC_GAIN,
	SCPI_ANC_PARAMETER_DATA,
}scpi_anc_parameter_e;




//scpi 
int app_user_spp_scpi_init(void);


#endif
