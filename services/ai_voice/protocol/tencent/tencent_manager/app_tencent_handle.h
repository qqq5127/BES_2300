#ifndef __APP_TENCENT_SMARTVOICE_H_
#define __APP_TENCENT_SMARTVOICE_H_

#define TENCENT_DEVICE_NAME     "TENCENT_EARPHONE"
#define TENCENT_DEVICE_NAME_NUM 16

#define IS_ENABLE_START_DATA_XFER_STEP          0

#define IS_USE_ENCODED_DATA_SEQN_PROTECTION  0     //changed by liangjin
//#define IS_USE_ENCODED_DATA_SEQN_PROTECTION     1       //origal code 
#define IS_TENCENT_SMARTVOICE_DEBUG_ON                  0
#define IS_FORCE_STOP_VOICE_STREAM              0

#define APP_TENCENT_PATH_TYPE_HFP    (1 << 2)

#ifdef __cplusplus
extern "C" {
#endif

enum {
    APP_TENCENT_SMARTVOICE_STATE_IDLE,
    APP_TENCENT_SMARTVOICE_STATE_MIC_STARTING,
    APP_TENCENT_SMARTVOICE_STATE_MIC_STARTED,
    APP_TENCENT_SMARTVOICE_STATE_SPEAKER_STARTING,
    APP_TENCENT_SMARTVOICE_STATE_SPEAKER_STARTED,
    APP_TENCENT_SMARTVOICE_STATE_MAX
};

enum {
    APP_TENCENT_SMARTVOICE_MESSAGE_ID_CUSTOMER_FUNC,
    APP_TENCENT_SMARTVOICE_MESSAGE_ID_MAX
};

//#define APP_TENCENT_PID_DATA   "\xCF\x75\x2B\x7D" 
#define  APP_TENCENT_PID_DATA   "\xB2\x76\x2B\x7D"
#define APP_TENCENT_PID_DATA_LEN  (4)

typedef struct
{
    uint8_t    bit;
    uint8_t    operating_sys;  
    uint8_t    flag_value[32];
}__attribute__ ((__packed__))tencent_config_t;
typedef enum {
    xw_headphone_extra_config_key_unknown = 0,

    xw_headphone_extra_config_key_keyFunction = 100,
    xw_headphone_extra_config_key_skuid = 101, 

    xw_headphone_extra_config_key_customizedName = 200, 
} xw_headphone_extra_config_key;

typedef enum {
    xw_headphone_extra_config_value_keyFunction_unknown = 0,
    xw_headphone_extra_config_value_keyFunction_default = 1, 
    xw_headphone_extra_config_value_keyFunction_xiaowei = 2, 
} xw_headphone_extra_config_value_keyFunction;
	
typedef struct _xw_headphone_extra_config_item {
	xw_headphone_extra_config_key key;
	xw_headphone_extra_config_value_keyFunction value;
} xw_headphone_extra_config_item;


typedef struct {
    uint8_t     transport_mode;     //spp/ble;
    uint8_t     encode_format;      
    uint8_t     compression_ration; 
	uint8_t     sample_rate;        //sample rate；
	uint8_t     mic_channel_count;  //recording channel；
    uint8_t     bit_number;         //bit depth；
    uint8_t     record_method;      //recording mode-- wakeup method
    uint32_t    firmware_version;   //software version;
    uint8_t     param_length;       //earphone name length；
	uint8_t     param[16];          // earphone name
	uint8_t		protocol_version;
	xw_headphone_extra_config_item extra_data;
	uint8_t		extra_data_length;
} __attribute__((packed)) EARPHONE_CONFIG_INFO_T;


extern uint8_t  conn_tag;
extern const struct ai_handler_func_table tencent_handler_function_table;

typedef void (*APP_TENCENT_SMARTVOICE_MOD_HANDLE_T)(void);


void app_tencent_sv_start_voice_stream_via_hfp(void);
int app_tencent_sv_start_voice_stream_via_ble(void);
int app_tencent_sv_start_voice_stream_via_spp(void);

int app_tencent_smartvoice_start_mic_stream(void);
int app_tencent_smartvoice_stop_mic_stream(void);
void app_tencent_smartvoice_switch_hfp_stream_status(void);
void app_tencent_smartvoice_switch_ble_stream_status(void);
void app_tencent_smartvoice_switch_spp_stream_status(void);
void tencent_wakeup_app(void);
void  tencent_double_click_to_stop_app_playing(void);
uint32_t tencent_loose_button_to_request_for_stop_record_voice_stream(void *param1, uint32_t param2);

void app_tencent_sv_throughput_test_init(void);
void app_tencent_sv_stop_throughput_test(void);
void app_tencent_sv_set_throughput_test_conn_parameter(uint8_t conidx, uint16_t minConnIntervalInMs, uint16_t maxConnIntervalInMs);
uint32_t app_tencent_sv_throughput_test_transmission_handler(void *param1, uint32_t param2);
#ifdef __cplusplus
}
#endif

#endif

