#if defined(__SCPI_TEST__)
/**
 * @file   app_scpi_test.c
 * @date   Thu Nov 15 10:58:45 UTC 2019
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_scpi_stub.h"
#include "app_scpi_test.h"

//anc
#include "parser_private.h"
#include "app_spp_anc.h"
#include "app_anc.h"
#include "app_spp.h"
#include "aud_section.h"
#include "crc32.h"
#include "tgt_hardware.h"
#include "hal_norflash.h"
#include "cmsis.h"
#ifdef __ARMCC_VERSION
#include "link_sym_armclang.h"
#endif
#include "ui_event_manage.h"
#include "ui_event_scpi.h"
#include "ui_event_product_test.h"
#include "ui_event_vbus.h"

extern void app_anc_status_sync(uint8_t status);

extern uint32_t __aud_start[];

extern const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_48k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_44p1k[ANC_COEF_LIST_NUM];

#ifdef	__ZOWEE_ANC_TEST_VIA_SPP__
static pctool_aud_section anc_section_copy;
#endif

size_t SCPI_Write(scpi_t * context, const char * data, size_t len);
int SCPI_Error(scpi_t * context, int_fast16_t err);
scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
scpi_result_t SCPI_Reset(scpi_t * context);
scpi_result_t SCPI_Flush(scpi_t * context);

static bool scpi_test_init_done = false;

extern const scpi_command_t scpi_commands[];
extern scpi_interface_t scpi_interface;
extern char scpi_input_buffer[];
extern scpi_error_t scpi_error_queue_data[];
extern scpi_t scpi_context;

extern void app_anc_gain_fadeout(void);
extern int anc_load_cfg(void);

/*scpi buffer*/
char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
scpi_t scpi_context;




scpi_result_t scpi_cls_spp_send_data(uint8_t * buff,uint16_t length)
{
	buff[length] = 0x0A;
	app_spp_user_send_data(buff,length+1);
	osDelay(25);
	return SCPI_RES_OK;
}


#ifdef	__ZOWEE_ANC_TEST_VIA_SPP__

/* ANC system command*/
static uint32_t scpi_store_anc_flash(void)
{
    uint32_t crc = 0,len;
	enum HAL_NORFLASH_RET_T flash_opt_res;
	uint32_t addr_start = 0;
	const pctool_aud_section *audsec_ptr;

	len = sizeof(anc_section_copy.sec_body) - 4;
    anc_section_copy.sec_head.magic = aud_section_magic;
    anc_section_copy.sec_head.version = aud_section_struct_version;

     //calculate crc
    crc = crc32(0, (unsigned char *)(&anc_section_copy.sec_body), len);
    anc_section_copy.sec_head.crc = crc;
	

	addr_start = (uint32_t)__aud_start;
	audsec_ptr = (pctool_aud_section *)__aud_start;

	//APP_SCPI_TRACE("crc11 %x %x %x",crc,len,sizeof(anc_section_copy));
	//crc = crc32(0, (unsigned char *)(&audsec_ptr->sec_body), len);
	//APP_SCPI_TRACE("crc22 %x %x %x",crc,len,sizeof(anc_section_copy));

	
	uint32_t flag = int_lock();
	flash_opt_res = hal_norflash_erase(HAL_NORFLASH_ID_0, addr_start, sizeof(anc_section_copy));
	int_unlock(flag);
	

	//APP_SCPI_TRACE("48k total gain111 %d",audsec_ptr->sec_body.anc_config.anc_config_arr[0].anc_cfg[PCTOOL_SAMPLERATE_48X8K].anc_cfg_ff_l.total_gain);
	//crc = crc32(0, (unsigned char *)(&audsec_ptr->sec_body), len);
	//APP_SCPI_TRACE("crc33 %x %x %x",crc,len,sizeof(anc_section_copy));
	if (flash_opt_res)
	{
		APP_SCPI_TRACE("ERROR: erase flash res = %d", flash_opt_res);
		return 0;
	}
	
	flag = int_lock();
	flash_opt_res = hal_norflash_write(HAL_NORFLASH_ID_0, addr_start, (uint8_t *)(&anc_section_copy), sizeof(anc_section_copy));
	int_unlock(flag);
	
	if (flash_opt_res)
	{
		APP_SCPI_TRACE("ERROR: write flash res = %d", flash_opt_res);
		return 0;
	}
	//APP_SCPI_TRACE("48k total gain222 %d",audsec_ptr->sec_body.anc_config.anc_config_arr[0].anc_cfg[PCTOOL_SAMPLERATE_48X8K].anc_cfg_ff_l.total_gain);

	//audsec_ptr = (pctool_aud_section *)__aud_start;
	//crc = crc32(0, (unsigned char *)(&audsec_ptr->sec_body), len);
	//APP_SCPI_TRACE("crc3 %x %x",crc,len);
	//APP_SCPI_TRACE("48k total gain %d",anc_coef_list_48k[index]->anc_cfg_ff_l.total_gain);	
	//APP_SCPI_TRACE("48k total gain %d",audsec_ptr->sec_body.anc_config.anc_config_arr[0].anc_cfg[PCTOOL_SAMPLERATE_48X8K].anc_cfg_ff_l.total_gain);

	//APP_SCPI_TRACE("48k total gain %d",audsec_ptr->sec_body.anc_config.anc_config_arr[0].anc_cfg[PCTOOL_SAMPLERATE_48X8K].anc_cfg_ff_l.total_gain);
    crc = crc32(0, (unsigned char *)(&audsec_ptr->sec_body), len);
	APP_SCPI_TRACE("crc44 %x",crc);
    crc = crc32(0, (unsigned char *)(&anc_section_copy.sec_body), len);
    APP_SCPI_TRACE("crc55 %x",crc);
	return crc;
}


scpi_result_t scpi_cls_anc_get_state_handle(scpi_t * context)
{
	uint8_t anc_status_buff[SCPI_MAILBOX_BUFF_MAX];
	const char *buff;

	memset(anc_status_buff,0,SCPI_MAILBOX_BUFF_MAX);
	buff = context->buffer.data;
	memcpy(anc_status_buff,buff,9);
	
	anc_status_buff[9] = ' ';
	anc_status_buff[10] = app_anc_get_anc_status() + '0';
	scpi_cls_spp_send_data(&anc_status_buff[0],11);
	
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_state_handle(scpi_t * context)
{
	bool value_return = false;


	SCPI_ParamBool(context,&value_return,true);
	APP_SCPI_TRACE(": %d", value_return);
	
	if(value_return == true)
	{
#if defined(IBRT)
		app_anc_status_sync(1);
#endif
		app_anc_status_post(1);
	}
	else
	{
		app_anc_gain_fadeout();

	}
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_get_mode_handle(scpi_t * context)
{
	uint8_t anc_mode_index_buff[SCPI_MAILBOX_BUFF_MAX];
	const char *buff;

	memset(anc_mode_index_buff,0,SCPI_MAILBOX_BUFF_MAX);
	buff = context->buffer.data;
	memcpy(anc_mode_index_buff,buff,8);

	anc_mode_index_buff[8] = ' '; 
	anc_mode_index_buff[9] = app_anc_get_coef() + '0';
	anc_mode_index_buff[9]++;
	scpi_cls_spp_send_data(&anc_mode_index_buff[0],10);
	return SCPI_RES_OK;

}

scpi_result_t scpi_cls_anc_mode_handle(scpi_t * context)
{
	uint8_t anc_mode_index;

	SCPI_ParamUInt32(context,(uint32_t *)(&anc_mode_index),true);
	APP_SCPI_TRACE(": %d", anc_mode_index);
	anc_mode_index--;
	
	if(anc_mode_index < ANC_COEF_LIST_NUM)
	{
		app_anc_set_coef(anc_mode_index);

		//close anc
#if defined(IBRT)
		app_anc_status_sync(0);
#endif
		app_anc_status_post(0);

		//open anc
#if defined(IBRT)
		app_anc_status_sync(1);
#endif
		app_anc_status_post(1);
	}
	return SCPI_RES_OK;
}


scpi_result_t scpi_cls_anc_sync_handle(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_data_preview_handle(scpi_t * context)
{
	uint8_t i;
	
#ifdef __AUDIO_RESAMPLE__
    for(i=0;i<ANC_COEF_LIST_NUM;i++)
	{
		anc_coef_list_50p7k[i] = &anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K];
		anc_coef_list_48k[i] = &anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K];
	}
#else
    for(i=0;i<ANC_COEF_LIST_NUM;i++)
	{
		anc_coef_list_48k[i] = &anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K];
		anc_coef_list_44p1k[i] = &anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K];
	}
#endif
	//close anc
	app_anc_gain_fadeout();
	osDelay(200);
	//open anc
#if defined(IBRT)
	app_anc_status_sync(1);
#endif
	app_anc_status_post(1);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_burn_handle(scpi_t * context)
{	
	scpi_store_anc_flash();

#if 0
	crc_calc = crc32(0,(unsigned char *)&(audsec_ptr->sec_body),sizeof(audsec_body)-4);

	{//debug
		uint8_t index = app_anc_get_coef();
		
		APP_SCPI_TRACE("index %d ",index);
#ifdef __AUDIO_RESAMPLE__
		APP_SCPI_TRACE("50p7k total gain %d",anc_coef_list_50p7k[index]->anc_cfg_ff_l.total_gain);	
		APP_SCPI_TRACE("50p7k iir_bypass_flag %d",anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_bypass_flag);	
		APP_SCPI_TRACE("50p7k iir_counter %d",anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_counter);	
		APP_SCPI_TRACE("50p7k iir_coef1 %x %x %x",anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[0],anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[1],anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[2]);	
		APP_SCPI_TRACE("50p7k iir_coef2 %x %x %x",anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[0],anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[1],anc_coef_list_50p7k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[2]);	
#endif
	
		APP_SCPI_TRACE("48k total gain %d",anc_coef_list_48k[index]->anc_cfg_ff_l.total_gain);	
		APP_SCPI_TRACE("48k iir_bypass_flag %d",anc_coef_list_48k[index]->anc_cfg_ff_l.iir_bypass_flag);	
		APP_SCPI_TRACE("48k iir_counter %d",anc_coef_list_48k[index]->anc_cfg_ff_l.iir_counter);	
		APP_SCPI_TRACE("48k iir_coef1 %x %x %x",anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[0],anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[1],anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[2]);	
		APP_SCPI_TRACE("48k iir_coef2 %x %x %x",anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[0],anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[1],anc_coef_list_48k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[2]);	
	
		APP_SCPI_TRACE("44p1k total gain %d",anc_coef_list_44p1k[index]->anc_cfg_ff_l.total_gain);	
		APP_SCPI_TRACE("44p1k iir_bypass_flag %d",anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_bypass_flag);	
		APP_SCPI_TRACE("44p1k iir_counter %d",anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_counter);	
		APP_SCPI_TRACE("44p1k iir_coef1 %x %x %x",anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[0],anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[1],anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_b[2]);	
		APP_SCPI_TRACE("44p1k iir_coef2 %x %x %x",anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[0],anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[1],anc_coef_list_44p1k[index]->anc_cfg_ff_l.iir_coef[0].coef_a[2]);	
	
	
	}
	APP_SCPI_TRACE("crc %x %x",crc,crc_calc);
#endif	
	return SCPI_RES_OK;
}


static void scpi_cls_send_anc_buff_with_info(uint8_t k,uint8_t j,uint8_t *anc_parameter_buff,uint8_t anc_parameter_count)
{
	//add FF|FB|EQ|MC, LEFT|RIGHt ,50.7|48|44.1
	if(k < 2)
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"FF,",3);
	}
	else if(k < 4)
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"FB,",3);
	}
	else if(k < 6)
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"EQ,",3);
	}
	else
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"MC,",3);
	}
	anc_parameter_count += 3;

	if(k %2 == 0)
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"LEFT,",5);
		anc_parameter_count += 5;
	}
	else
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"RIGHt,",6);
		anc_parameter_count += 6;
	}

	if(j == 0)
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"48",2);
		anc_parameter_count += 2;
	}else
	{
		memcpy(&(anc_parameter_buff[anc_parameter_count]),"44.1",4);
		anc_parameter_count += 4;
	}

	scpi_cls_spp_send_data(&anc_parameter_buff[0],anc_parameter_count);

}

static int scpi_cls_send_anc_parameter(scpi_anc_parameter_e parameter)
{

	for(uint8_t i = 0;i < ANC_COEF_LIST_NUM;i++)
	{
		for(uint8_t j = 0;j < 2;j++)
		{
			const struct_anc_cfg *anc_coef_list;
			uint8_t anc_parameter_struct_count;
			uint8_t k;

#ifdef __AUDIO_RESAMPLE__
			if(j == 0)
			{
				anc_coef_list = anc_coef_list_50p7k[i];
			}
			else if(j == 1)
			{
				anc_coef_list = anc_coef_list_48k[i];
			}
#else
			if(j == 0)
			{
				anc_coef_list = anc_coef_list_48k[i];
			}
			else if(j == 1)
			{
				anc_coef_list = anc_coef_list_44p1k[i];
			}

#endif
			aud_item *anc_cfg_point;

			anc_cfg_point = (aud_item *)(&(anc_coef_list->anc_cfg_ff_l));
			anc_parameter_struct_count = sizeof(struct_anc_cfg)/sizeof(aud_item);
			
			for(k=0;k<anc_parameter_struct_count;k++)
			{
				uint8_t anc_parameter_buff[SCPI_MAILBOX_BUFF_MAX];
				uint8_t anc_parameter_count = 0;

				memset(anc_parameter_buff,0,SCPI_MAILBOX_BUFF_MAX);
				switch(parameter)
				{
					case SCPI_ANC_PARAMETER_CONTROL:
						{
							//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_CONTROL %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
							memcpy(anc_parameter_buff,"ANC:CONTrol ",12);
							anc_parameter_count = 12;
							
							//add reslut
							if(anc_cfg_point->total_gain == 0)
							{
								anc_parameter_buff[anc_parameter_count] = '0';
							}
							else 
							{
								anc_parameter_buff[anc_parameter_count] = '1';
							}
							anc_cfg_point = anc_cfg_point + 1;
							anc_parameter_count++;
							anc_parameter_buff[anc_parameter_count] = ',';
							anc_parameter_count++;
						}
				
						break;

					case SCPI_ANC_PARAMETER_TOTAL_GAIN:
						{
							memcpy(anc_parameter_buff,"ANC:TOTal:GAIN ",15);
							anc_parameter_count = 15;
							//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
							//add reslut
							//sprintf((char*)&tmp[i], "%c", (char)ph->tag[i]);
							
							sprintf((char *)(anc_parameter_buff+15),"%d",anc_cfg_point->total_gain);
							anc_cfg_point = anc_cfg_point + 1;

							for(anc_parameter_count = 0;anc_parameter_count<SCPI_MAILBOX_BUFF_MAX;anc_parameter_count++)
							{
								if(anc_parameter_buff[anc_parameter_count] == 0)
								{
									anc_parameter_buff[anc_parameter_count] = ',';
									anc_parameter_count++;
									break;
								}
							}

						}
						break;
						
					case SCPI_ANC_PARAMETER_BYPASS:
						{
							memcpy(anc_parameter_buff,"ANC:BYPAss ",11);
							anc_parameter_count = 11;
							//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
							//add reslut
							//sprintf((char*)&tmp[i], "%c", (char)ph->tag[i]);
							
							sprintf((char *)(anc_parameter_buff+11),"%x",anc_cfg_point->iir_bypass_flag);
							anc_cfg_point = anc_cfg_point + 1;

							for(anc_parameter_count = 0;anc_parameter_count<SCPI_MAILBOX_BUFF_MAX;anc_parameter_count++)
							{
								if(anc_parameter_buff[anc_parameter_count] == 0)
								{
									anc_parameter_buff[anc_parameter_count] = ',';
									anc_parameter_count++;
									break;
								}
							}

						}
						break;
					case SCPI_ANC_PARAMETER_COUNT:
						{
							memcpy(anc_parameter_buff,"ANC:COUNt ",10);
							anc_parameter_count = 10;
							//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
							//add reslut
							//sprintf((char*)&tmp[i], "%c", (char)ph->tag[i]);
							
							sprintf((char *)(anc_parameter_buff+10),"%x",anc_cfg_point->iir_counter);
							anc_cfg_point = anc_cfg_point + 1;

							for(anc_parameter_count = 0;anc_parameter_count<SCPI_MAILBOX_BUFF_MAX;anc_parameter_count++)
							{
								if(anc_parameter_buff[anc_parameter_count] == 0)
								{
									anc_parameter_buff[anc_parameter_count] = ',';
									anc_parameter_count++;
									break;
								}
							}

						}
						break;

					case SCPI_ANC_PARAMETER_ADC_GAIN:
						{
							memcpy(anc_parameter_buff,"ANC:ADC:GAIN ",13);
							anc_parameter_count = 13;
							//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
							//add reslut
							//sprintf((char*)&tmp[i], "%c", (char)ph->tag[i]);
							
							sprintf((char *)(anc_parameter_buff+13),"%d",anc_cfg_point->adc_gain_offset);
							anc_cfg_point = anc_cfg_point + 1;

							for(anc_parameter_count = 0;anc_parameter_count<SCPI_MAILBOX_BUFF_MAX;anc_parameter_count++)
							{
								if(anc_parameter_buff[anc_parameter_count] == 0)
								{
									anc_parameter_buff[anc_parameter_count] = ',';
									anc_parameter_count++;
									break;
								}
							}

						}
						break;

					case SCPI_ANC_PARAMETER_DATA:
						{
							uint8_t m;
							int sprintf_len = 0;
							uint8_t n = 0;

							APP_SCPI_TRACE("anc_parameter_struct_count %d %d %d",anc_parameter_struct_count,k,anc_cfg_point->iir_counter);
							for(m = 0;m < anc_cfg_point->iir_counter;m++)
							{
								memset(anc_parameter_buff,0,sizeof(anc_parameter_buff));
								memcpy(anc_parameter_buff,"ANC:DATA ",9);
								anc_parameter_count = 9;
								//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN %d %d %d %d,anc_cfg_point->total_gain %x", anc_parameter_count,j,i,k,anc_cfg_point->total_gain);
								//add reslut
								//sprintf((char*)&tmp[i], "%c", (char)ph->tag[i]);

								for(n=0;n<6;n++)
								{
									if(n<3)
									{
										sprintf_len = sprintf((char *)(anc_parameter_buff+anc_parameter_count),"0x%x",anc_cfg_point->iir_coef[m].coef_b[n]);
									}
									else
									{
										sprintf_len = sprintf((char *)(anc_parameter_buff+anc_parameter_count),"0x%x",anc_cfg_point->iir_coef[m].coef_a[n-3]);
									}
									anc_parameter_count += sprintf_len;
									anc_parameter_buff[anc_parameter_count] = ',';
									anc_parameter_count++;
								}
								anc_parameter_buff[anc_parameter_count] = '1' + m;
								anc_parameter_count++;
								anc_parameter_buff[anc_parameter_count] = ',';
								anc_parameter_count++;
								scpi_cls_send_anc_buff_with_info(k,j,anc_parameter_buff,anc_parameter_count);
							}
							anc_cfg_point = anc_cfg_point + 1;
						}
						break;
						
					default:

						break;
				}
			if(SCPI_ANC_PARAMETER_DATA != parameter)	
			{
				scpi_cls_send_anc_buff_with_info(k,j,anc_parameter_buff,anc_parameter_count);
			}
			}
		}
	}
	
	return 0;
}

static int scpi_cls_set_anc_parameter(scpi_anc_parameter_e parameter,const char *buff,uint16_t lenght)
{
	char *str_compare_point;
	scpi_sampling_rate_e scpi_set_struct_sampling_rate = SCPI_SAMPLING_RATE_ERROR; 
	scpi_channel_e scpi_set_struct_channel = SCPI_CHANNEL_ERROR; 
	scpi_mic_channel_e scpi_set_struct_mic_channel = SCPI_MIC_CHANNEL_ERROR; 
	aud_item *anc_cfg_point;
	const struct_anc_cfg *anc_coef_list;

	APP_SCPI_TRACE(": %s \n", buff);
	
	buff = strchr((const char *)buff,' ');
	buff += 2;
	
	//parse sampling rate
	str_compare_point = strrchr((const char *)buff,',');
	str_compare_point++;
	if(memcmp(str_compare_point,"50.7",4) == 0)
	{
		scpi_set_struct_sampling_rate = SCPI_SAMPLING_RATE_50P7K;
	}
	else if(memcmp(str_compare_point,"48",2) == 0)
	{
		scpi_set_struct_sampling_rate = SCPI_SAMPLING_RATE_48K;
	}
	else if(memcmp(str_compare_point,"44.1",4) == 0)
	{
		scpi_set_struct_sampling_rate = SCPI_SAMPLING_RATE_44P1K;
	}
	APP_SCPI_TRACE("%s scpi_set_struct_sampling_rate %d\n", str_compare_point, scpi_set_struct_sampling_rate);

	if(scpi_set_struct_sampling_rate == SCPI_SAMPLING_RATE_ERROR)
	{
		return -1;
	}
	

	//parse LEFT RIGHT
	str_compare_point--;
	*str_compare_point = '\0';
	str_compare_point = strrchr((const char *)buff,',');
	str_compare_point++;
	if(memcmp(str_compare_point,"LEFT",4) == 0)
	{
		scpi_set_struct_channel = SCPI_CHANNEL_LEFT;
	}
	else if(memcmp(str_compare_point,"RIGHt",5) == 0)
	{
		scpi_set_struct_channel = SCPI_CHANNEL_RIGHT;
	}
	APP_SCPI_TRACE("%s scpi_set_struct_sampling_rate %d\n", str_compare_point, scpi_set_struct_channel);

	if(scpi_set_struct_channel == SCPI_CHANNEL_ERROR)
	{
		return -1;
	}

	//parse FF FB EQ MC
	str_compare_point--;
	*str_compare_point = '\0';
	str_compare_point = strrchr((const char *)buff,',');
	str_compare_point++;
	if(memcmp(str_compare_point,"FF",2) == 0)
	{
		scpi_set_struct_mic_channel = SCPI_MIC_CHANNEL_FF;
	}
	else if(memcmp(str_compare_point,"FB",2) == 0)
	{
		scpi_set_struct_mic_channel = SCPI_MIC_CHANNEL_FB;
	}
	else if(memcmp(str_compare_point,"EQ",2) == 0)
	{
		scpi_set_struct_mic_channel = SCPI_MIC_CHANNEL_EQ;
	}
	else if(memcmp(str_compare_point,"MC",2) == 0)
	{
		scpi_set_struct_mic_channel = SCPI_MIC_CHANNEL_MC;
	}
	APP_SCPI_TRACE("%s scpi_set_struct_mic_channel %d\n", str_compare_point, scpi_set_struct_mic_channel);

	if(scpi_set_struct_mic_channel == SCPI_MIC_CHANNEL_ERROR)
	{
		return -1;
	}
	// location the point ,and set the value

	if(scpi_set_struct_sampling_rate == SCPI_SAMPLING_RATE_48K)
	{
		anc_coef_list = &(anc_section_copy.sec_body.anc_config.anc_config_arr[app_anc_get_coef()].anc_cfg[PCTOOL_SAMPLERATE_48X8K]);
	}
	else if(scpi_set_struct_sampling_rate == SCPI_SAMPLING_RATE_44P1K)
	{
		anc_coef_list = &(anc_section_copy.sec_body.anc_config.anc_config_arr[app_anc_get_coef()].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K]);
	}
	else
	{
		return SCPI_RES_OK;
	}
	anc_cfg_point = (aud_item *)(&(anc_coef_list->anc_cfg_ff_l));
	anc_cfg_point = anc_cfg_point + (uint8_t )(scpi_set_struct_mic_channel*2 + scpi_set_struct_channel);

	switch(parameter)
	{
		case SCPI_ANC_PARAMETER_CONTROL:
			{
				uint32_t value_hex;

				strBaseToUInt32(buff,&value_hex,10);
				if(value_hex == 0)
				{
					anc_cfg_point->total_gain = 0;
				}
				else
				{
					anc_cfg_point->total_gain = 512;
				}
				APP_SCPI_TRACE("anc_cfg_point->total_gain %d",value_hex);
				break;

			}
		case SCPI_ANC_PARAMETER_TOTAL_GAIN:
			{
				uint16_t data_length;

				
				data_length = (uint16_t)(strBaseToInt32(buff,&anc_cfg_point->total_gain,10));
				APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN_SET %d %d \n",anc_cfg_point->total_gain,data_length);
			}
			
			break;
		case SCPI_ANC_PARAMETER_BYPASS:
			{
				uint32_t value_hex;
				
				strBaseToUInt32(buff,&value_hex,10);
				if(value_hex == 0)
				{
					anc_cfg_point->iir_bypass_flag = 0;
				}
				else
				{
					anc_cfg_point->iir_bypass_flag = 1;
				}
				APP_SCPI_TRACE("anc_cfg_point->iir_bypass_flag %d",value_hex);
			}
			break;

		case SCPI_ANC_PARAMETER_COUNT:
			{
				uint32_t value_hex;
				
				strBaseToUInt32(buff,&value_hex,10);
				anc_cfg_point->iir_counter = (uint16_t )value_hex;
				APP_SCPI_TRACE("anc_cfg_point->iir_counter %d",anc_cfg_point->iir_counter);
			}
			break;

		case SCPI_ANC_PARAMETER_ADC_GAIN:
			{
			uint16_t data_length;
		
			
			data_length = (uint16_t)(strBaseToInt32(buff,(int32_t *)(&anc_cfg_point->adc_gain_offset),10));
			APP_SCPI_TRACE("SCPI_ANC_PARAMETER_TOTAL_GAIN_SET %d %d \n",anc_cfg_point->adc_gain_offset,data_length);
			}
			break;
			
		case SCPI_ANC_PARAMETER_DATA:
			{
			uint16_t data_length;
			int32_t count,m;
			
			str_compare_point--;
			*str_compare_point = '\0';
			str_compare_point = strrchr((const char *)buff,',');
			str_compare_point++;
			data_length = (uint16_t)(strBaseToInt32(str_compare_point,(int32_t *)(&count),16));

			str_compare_point = (char *)buff;
			count--;
			APP_SCPI_TRACE("str_compare_point %s", str_compare_point);
			
			for(m=0;m<6;m++)
			{
				if(m<3)
				{
					data_length = (uint16_t)(strBaseToUInt32((str_compare_point),(uint32_t *)(&anc_cfg_point->iir_coef[count].coef_b[m]),16));
					APP_SCPI_TRACE("anc_cfg_point->iir_coef[%d].coef_b[%d] %x",count,m,anc_cfg_point->iir_coef[count].coef_b[m]);
				}
				else
				{
					data_length = (uint16_t)(strBaseToUInt32((str_compare_point),(uint32_t *)(&anc_cfg_point->iir_coef[count].coef_a[m-3]),16));
					APP_SCPI_TRACE("anc_cfg_point->iir_coef[%d].coef_a[%d] %x",count,m-3,anc_cfg_point->iir_coef[count].coef_a[m-3]);
				}
				str_compare_point = str_compare_point + data_length + 1;
			}
			//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_DATA 1 %d %d %d \n",anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_b[0],anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_b[1],anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_b[2]);
			//APP_SCPI_TRACE("SCPI_ANC_PARAMETER_DATA 2 %d %d %d \n",anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_a[0],anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_a[1],anc_coef_list->anc_cfg_ff_l.iir_coef[0].coef_a[2]);

			}
			break;
		break;
			
		default:

			break;
	}
	
	//scpi_store_anc_flash();
	//APP_SCPI_TRACE("total_gain %d\n", anc_coef_list_50p7k[app_anc_get_coef()]->anc_cfg_ff_l.total_gain);
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_get_control_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_CONTROL);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_control_handle(scpi_t * context)
{			
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_CONTROL,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_get_total_gain_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_TOTAL_GAIN);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_total_gain_handle(scpi_t * context)
{
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_TOTAL_GAIN,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_get_bypass_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_BYPASS);
	
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_bypass_handle(scpi_t * context)
{
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_BYPASS,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;

}

scpi_result_t scpi_cls_anc_get_count_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_COUNT);
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_count_handle(scpi_t * context)
{
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_COUNT,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;

}

scpi_result_t scpi_cls_anc_get_adc_gain_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_ADC_GAIN);
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_adc_gain_handle(scpi_t * context)
{
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_ADC_GAIN,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_get_data_handle(scpi_t * context)
{
	scpi_cls_send_anc_parameter(SCPI_ANC_PARAMETER_DATA);
	return SCPI_RES_OK;
}

scpi_result_t scpi_cls_anc_data_handle(scpi_t * context)
{
	scpi_cls_set_anc_parameter(SCPI_ANC_PARAMETER_DATA,context->buffer.data,context->buffer.position);

	return SCPI_RES_OK;

}
#endif

/* command sets*/
const scpi_command_t scpi_commands[] = {
	/* IEEE Mandated Commands (SCPI std V1999.0 4.1.1)  for earbud test*/
	{ .pattern = "*CLS", .callback = SCPI_CoreCls,},
	{ .pattern = "*IDN?", .callback = SCPI_Read_IDN,},
	{ .pattern = "*OPC", .callback = SCPI_CoreOpc,},
	{ .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
	{ .pattern = "*RST", .callback = SCPI_Reoot,},
	{ .pattern = "*SAV", .callback = NULL,},//add after
		
	/* SYSTem SCPI commands */
	{.pattern = ":SYSTem:REBoot", .callback = SCPI_Reoot,},
	{.pattern = ":SYSTem:SHUT:DOWN", .callback = SCPI_Shut_Down,},
	{.pattern = ":SYSTem[:FACTory]:RESet", .callback = SCPI_Factory_Reset,},
	{.pattern = ":SYSTem:SW:VERSion?", .callback = SCPI_Get_SW_Version,},
	{.pattern = ":SYSTem:HW:VERSion?", .callback = SCPI_Get_HW_Version,},//add after
	{.pattern = ":SYSTem:BT:ADDRess?", .callback = SCPI_Get_BT_Address,},
	{.pattern = ":SYSTem:BT:NAME?", .callback = SCPI_Get_BT_Name,},
	{.pattern = ":SYSTem:TOUChing:STATe?", .callback = SCPI_Get_Touch_State,},
	{.pattern = "[:SYSTem]:BOX:STATe", .callback = SCPI_Set_Box_State,},
	{.pattern = "[:SYSTem]:WEAR:STATe", .callback = SCPI_Set_Wear_State,},
	{.pattern = ":SYSTem:STATe?", .callback = SCPI_Get_Sys_State,},
	{.pattern = ":SYSTem[:EARBuds]:ROLE", .callback = SCPI_Set_Role,},
	{.pattern = ":SYSTem[:EARBuds]:ROLE?", .callback = SCPI_Get_Role,},
	{.pattern = ":SYSTem[:EARBuds]:SWITch:ROLE", .callback = SCPI_Switch_Role,},
	{.pattern = ":SYSTem:TEMPerature?", .callback = SCPI_Get_Temperature,},//add after
	{.pattern = "[:SYSTem]:SCPI:MODE", .callback = SCPI_SCPI_Mode,},
	{.pattern = "[:SYSTem]:SCPI:MODE?", .callback = SCPI_Get_SCPI_Mode,},
	{.pattern = "[:SYSTem]:SCPI:HEAD", .callback = SCPI_Return_Head,},
	{.pattern = "[:SYSTem]:PROJect:NAME?", .callback = SCPI_Project_Name,},
	
	/*BATTERY sub system command*/ 
	{.pattern = ":BATTery:CHARge?", .callback = SCPI_Get_Charge_State,},
	{.pattern = ":BATTery:LEVEl?", .callback = SCPI_Get_Battery_Level,},
	{.pattern = ":BATTery:VOLTage?", .callback = SCPI_Get_Battery_Voltage,},
		
	/*PAIRing sub system command*/
	{.pattern = ":PAIRing:MODE?", .callback = SCPI_Get_Pair_Mode,},
	{.pattern = ":PAIRing:PEER", .callback = SCPI_Set_Pair_Peer,},
	{.pattern = ":PAIRing:PEER?", .callback = SCPI_Get_Pair_Peer,},
	{.pattern = ":PAIRing:FREEman", .callback = SCPI_Set_Freeman_Pair,},
	{.pattern = ":PAIRing:HANDSet", .callback = SCPI_Set_Pair_Phone,},
	{.pattern = ":PAIRing:HANDSet?", .callback = SCPI_Get_Pair_Phone,},
	
	/*HFP sub system command*/
	//{.pattern = ":HFP:VERSion?", .callback = SCPI_Get_Hfp_Version,},//add after
	{.pattern = ":HFP:STATe?", .callback = SCPI_Get_HFP_Status,},
	{.pattern = ":HFP:VOLUme[:LEVEL]?", .callback = SCPI_Get_HFP_Volume,},

	/*MUSIc sub system command*/
	{.pattern = ":MUSIc:STATe?", .callback = SCPI_Get_Music_Status,},
	{.pattern = ":MUSIc:VOLUme[:LEVEL]?", .callback = SCPI_Get_Music_Volume,},

	/*AVRCp sub system command*/
	{.pattern = ":AVRCp:CONTrol:KEY:PRESs", .callback = SCPI_Control_Key,},

#ifdef	__ZOWEE_ANC_TEST_VIA_SPP__
	/* ANC sub system command*/
	{.pattern = ":ANC:STATe?", .callback = scpi_cls_anc_get_state_handle,},
	{.pattern = ":ANC:STATe", .callback = scpi_cls_anc_state_handle,},
	{.pattern = ":ANC:MODE?", .callback = scpi_cls_anc_get_mode_handle,},
	{.pattern = ":ANC:MODE", .callback = scpi_cls_anc_mode_handle,},
	{.pattern = ":ANC:DATA:PREView", .callback = scpi_cls_anc_data_preview_handle,},
	{.pattern = ":ANC:DATA:BURN", .callback = scpi_cls_anc_burn_handle,},
	{.pattern = ":ANC:CONTrol?", .callback = scpi_cls_anc_get_control_handle,},
	{.pattern = ":ANC:CONTrol", .callback = scpi_cls_anc_control_handle,},
	{.pattern = ":ANC:TOTal:GAIN?", .callback = scpi_cls_anc_get_total_gain_handle,},
	{.pattern = ":ANC:TOTal:GAIN", .callback = scpi_cls_anc_total_gain_handle,},
	{.pattern = ":ANC:BYPAss?", .callback = scpi_cls_anc_get_bypass_handle,},
	{.pattern = ":ANC:BYPAss", .callback = scpi_cls_anc_bypass_handle,},
	{.pattern = ":ANC:COUNt?", .callback = scpi_cls_anc_get_count_handle,},
	{.pattern = ":ANC:COUNt", .callback = scpi_cls_anc_count_handle,},
	{.pattern = ":ANC:ADC:GAIN?", .callback = scpi_cls_anc_get_adc_gain_handle,},
	{.pattern = ":ANC:ADC:GAIN", .callback = scpi_cls_anc_adc_gain_handle,},
	{.pattern = ":ANC:DATA?", .callback = scpi_cls_anc_get_data_handle,},
	{.pattern = ":ANC:DATA", .callback = scpi_cls_anc_data_handle,},
#endif		
	{.pattern = ":ANC:SWITch:MODE", .callback = SCPI_Switch_ANC_Mode,},
	{.pattern = ":ANC:SWITch:MODE?", .callback = SCPI_Get_ANC_Mode,},

	/* DUT sub system command*/
	{.pattern = ":DUT:SWITch:MIC?", .callback = SCPI_Switch_Mic,},
	{.pattern = ":DUT:SWITch:SCO:MIC?", .callback = SCPI_Switch_Sco_Mic,},
	{.pattern = ":DUT:MODE?", .callback = SCPI_Dut_Mode,},
#if defined(__IR_W2001__)
	{.pattern = ":DUT:IR[:VALUe]", .callback = SCPI_Write_IR_Value,},
	{.pattern = ":DUT:IR[:VALUe]?", .callback = SCPI_Read_IR_Value,},
	{.pattern = ":DUT:IR:CALIbration?", .callback = SCPI_IR_Cal,},
#endif //__IR_W2001__
	{.pattern = ":DUT:OTA", .callback = SCPI_OTA,},
	{.pattern = ":DUT:XTAL:CALIbration", .callback = SCPI_XTAL_Cal,},
	{.pattern = ":DUT:SCPI:MODE?", .callback = SCPI_DUT_SCPI_Mode,},
	{.pattern = ":DUT:PAIRing:FREEman?", .callback = SCPI_DUT_Set_Freeman_Pair,},
	{.pattern = ":DUT:PAIRing:PEER?", .callback = SCPI_DUT_Set_Pair_Peer,},
	{.pattern = ":DUT:REBoot?", .callback = SCPI_DUT_Reoot,},
	{.pattern = ":DUT:SHUT:DOWN?", .callback = SCPI_DUT_Shut_Down,},
	{.pattern = ":DUT:RESet?", .callback = SCPI_DUT_Factory_Reset,},
	{.pattern = ":DUT:ROLE?", .callback = SCPI_DUT_Set_Role,},
	{.pattern = ":DUT:WEAR_REPORT?", .callback = SCPI_DUT_Wear_Report,},
	SCPI_CMD_LIST_END
};

/*
	scpi operate
*/
scpi_interface_t scpi_interface = {
	.error = SCPI_Error,
	.write = SCPI_Write,
	.control = SCPI_Control,
	.flush = SCPI_Flush,
	.reset = SCPI_Reset,
};

static bool scpi_if_add_head(const char * data, size_t len)
{
	APP_SCPI_TRACE("data %d %c", data[0],data[0]);
	if((data[0] == '\r') && (data[1] == '\n'))
	{
		return false;
	}
	else if(data[0] == '"')
	{
		return false;
	}
	else if(data[0] == '\'')
	{
		return false;
	}
	else if(data[0] == '#')
	{
		return false;
	}
	else if(data[0] == ',')
	{
		return false;
	}
	else if(data[0] == ';')
	{
		return false;
	}
	return true;
}

/*
	scpi SCPI send data function
*/
size_t SCPI_Write(scpi_t * context, const char * data, size_t len)
{
	(void) context;
	char re_data[255];
	uint8_t head_len;
	int result;
	
	head_len = 0;
	result = scpi_if_add_head(data,len);
	if(result != 0 && (product_test_stauts.head_flag == 1))
	{//add head
		char *string_point;
		char *position_point;

		string_point = (char *)context->buffer.data;
		position_point = (char *)strrchr((const char *)string_point,' ');
		if(position_point == 0)
		{
			position_point = (char *)strrchr((const char *)string_point,'?');
		}
		if(position_point != 0)
		{
			head_len = (uint8_t )(position_point - string_point)/sizeof(char);
			head_len++;
			memcpy(re_data,string_point,head_len);
			APP_SCPI_TRACE(" %s %s len: %d %d %d",string_point, position_point,(uint32_t )string_point,(uint32_t )position_point,head_len);
			re_data[head_len - 1] = ' ';
		}
	}
	
	memcpy(re_data+head_len, data, len);
	len += head_len;

	if(product_test_stauts.spp_vbus_flag == 1)
	{
		ui_send_vbus_data((uint8_t *)re_data, len);
	}
	else
	{
		scpi_cls_spp_send_data((uint8_t *)re_data, len);
	}

	return SCPI_RES_OK;
}


/*
	scpi SCPI_Flush data function
*/
scpi_result_t SCPI_Flush(scpi_t * context)
{
	(void) context;

	return SCPI_RES_OK;
}

/*
	scpi SCPI error handle  function
*/
int SCPI_Error(scpi_t * context, int_fast16_t err)
{
	(void) context;
	/* BEEP */
	//fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
	return 0;
}

/*
	scpi SCPI control handle  function
*/
scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
	(void) context;

	if (SCPI_CTRL_SRQ == ctrl) {
   //	 fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
	} else {
   //	 fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
	}
	return SCPI_RES_OK;
}


/*
	scpi SCPI Reset handle  function
*/
scpi_result_t SCPI_Reset(scpi_t * context) {
	(void) context;

	fprintf(stderr, "**Reset\r\n");
	return SCPI_RES_OK;
}

/*
	set scpi context
*/
static void SetContext(void * Pfd)
{
	 scpi_context.user_context = Pfd;
}

/*
	scpi test init function
*/

static void scpi_acn_storage_review(void)
{
#ifdef	__ZOWEE_ANC_TEST_VIA_SPP__

    unsigned int re_calc_crc;
    const pctool_aud_section *audsec_ptr;
    
    audsec_ptr = (pctool_aud_section *)__aud_start;
	memcpy(&anc_section_copy,audsec_ptr,sizeof(pctool_aud_section));

    re_calc_crc = crc32(0,(unsigned char *)&(audsec_ptr->sec_body),sizeof(audsec_body)-4);
    if (re_calc_crc == audsec_ptr->sec_head.crc){
        APP_SCPI_TRACE("crc verify success, valid aud section.");
        return;
    }
	
	for(uint8_t i = 0;i<ANC_COEF_LIST_NUM;i++)
	{
#ifdef __AUDIO_RESAMPLE__
		memcpy(&anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K],anc_coef_list_48k[i],sizeof(struct_anc_cfg));
		memcpy(&anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K],anc_coef_list_50p7k[i],sizeof(struct_anc_cfg));
#else
		memcpy(&anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K],anc_coef_list_44p1k[i],sizeof(struct_anc_cfg));
		memcpy(&anc_section_copy.sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K],anc_coef_list_48k[i],sizeof(struct_anc_cfg));
#endif
	}
	
    scpi_store_anc_flash();
#endif
}

static scpi_data_staruct_t rx_data;

void Scpi_test_init(void)
{
	SetContext(NULL);

	scpi_acn_storage_review();
	memset(&rx_data,0,sizeof(rx_data));


	/*CALL SCPI LIB INIT*/
	SCPI_Init(&scpi_context,
			scpi_commands,
			&scpi_interface,
			scpi_units_def,
			SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
			scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
			scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

	scpi_test_init_done = true;	
	product_test_stauts.dut_flag = false;
}


/*
	scpi test init function
*/
void SCPI_Input_Fun(const char * data, int len)
{
	APP_SCPI_TRACE("%s, %d", data, len);
	if(!SCPI_Input(&scpi_context,data,len))
	{
		APP_SCPI_TRACE("TR SCPI: INSTRUCT INVALID!!!");
	}
	else
	{
		memset(&rx_data,0,sizeof(rx_data));
		memset(&vbus_rx_buff,0,sizeof(vbus_rx_buff));
	}
}

int app_scpi_test_spp_parse(uint8_t *cmd,uint8_t length)
{
	if (scpi_test_init_done)
	{
		if(NULL == cmd)
		{
			APP_SCPI_TRACE("TR SCPI: INVAILD POINT!!!");
			return 0;
		}
		if(length < 5)
		{
			APP_SCPI_TRACE("TR SCPI: INVAILD LEN!!!");
			return 0;
		}
		ASSERT((rx_data.length + length) < sizeof(rx_data.data), "app_scpi_test_spp_parse out of memery %d %d %d",rx_data.length,length,sizeof(rx_data.data));
		memcpy(rx_data.data + rx_data.length, cmd, length);
		rx_data.length += length;
		app_ui_event_process(UI_EVENT_SCPI,0,UI_EVENT_SCPI_SPP_PARSE,(uint32_t)rx_data.data,(uint32_t)(&rx_data.length));
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: Not Init!!!");
		return 0;
	}
	return 1;
}

int app_scpi_test_vbus_parse(uint8_t *cmd,uint8_t length)
{
	if (scpi_test_init_done)
	{
		if(NULL == cmd)
		{
			APP_SCPI_TRACE("TR SCPI: INVAILD POINT!!!");
			return 0;
		}
		if((rx_data.length + length) >= sizeof(rx_data.data))
		{
			memset(&rx_data,0,sizeof(rx_data));
		}
		memcpy(rx_data.data + rx_data.length, cmd, length);
		rx_data.length += length;
		if(ui_get_sys_status() == APP_UI_SYS_PRODUCT_TEST)
		{
			memset(&vbus_rx_buff,0,sizeof(vbus_rx_buff));
		}
		if(rx_data.length < 5)
		{
			APP_SCPI_TRACE("TR SCPI: INVAILD LEN!!!");
			return 0;
		}
		if(rx_data.data[rx_data.length - 1] == 0x0A)
		{
			app_ui_event_process(UI_EVENT_SCPI,0,UI_EVENT_SCPI_VBUS_PARSE,(uint32_t)rx_data.data,(uint32_t)(&rx_data.length));
		}
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: Not Init!!!");
		return 0;
	}
	return 1;
}


#endif

