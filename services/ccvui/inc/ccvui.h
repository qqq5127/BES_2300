#ifndef __CCVUI_H__
#define __CCVUI_H__

void getDeviceInfo(uint32_t* info);//info[4]

void ccvui_tx_init(int sample_rate, int frame_len);
void ccvui_tx_process(short *pcm_buf, short *ref_buf, int *_pcm_len);
void ccvui_tx_exit(void);

void *init_ClsNS_Handler(char *root, char *pmem, int *size,int *password);
void set_ClsNS_Gain(void *handler, int maxdb);
void make_ClsNS_Process(void *handler, short *inStream, short *outStream);

void *init_RnnNS_Handler(char *root, char *heap, int *size, int *password);
void set_RnnNS_MaxAttenu(void *handler,int maxdb);
void make_RnnNS_Process(void *handler, const short *in, short *out);

void *init_COHBF_Handler(char *root, char *pmem, int *size,int *password);
void set_COHBF_Param(void *handler, int mode,int distance,int degrees0,int degrees1);
void make_COHBF_Process(void *handler, short *stream_in1, short *stream_in0, short *stream_out);

#endif//__CCVUI_H__
