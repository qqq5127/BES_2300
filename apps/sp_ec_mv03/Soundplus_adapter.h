#ifndef __SOUNDPLUS_ADAPTER_H__
#define __SOUNDPLUS_ADAPTER_H__
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
    FunctionName:   Sndp_get_alg_ver
    Purpose:        get soundplus algorithm version
    Parameter:      
                    void
            
    Return:         char *:  point to version-string
******************************************************************************/
char *Sndp_get_alg_ver(void);



/***************************************************************
    FunctionName:   soundplus_auth
    Purpose:        soundplus algorithm authorize
    Parameter:
                    Key:
                    Key_len:
    Return:         return 1 if auth sucessed,else return error code
****************************************************************/
int soundplus_auth(uint8_t* key, int Key_len);
/***************************************************************
    FunctionName:   soundplus_auth_status
    Purpose:        get soundplus algorithm license state
    Parameter:
                    
    Return:         return 1 if auth sucessed,else return error code
****************************************************************/
int soundplus_auth_status();

/***************************************************************
    FunctionName:   soundplus_deal_Aec
    Purpose:        soundplus algorithm Tx AEC process
    Parameter:
                    out : mic pcm data out.
                    in : mic pcm data in.
                    ref : ref pcm data.
                    buf_len : mic pcm numsamples,
                          set 240*mic_num when BTIF_HF_SCO_CODEC_MSBC,
                          set 120*mic_num when BTIF_HF_SCO_CODEC_CVSD
                    ref_len : ref pcm numsamples,
                          set 240 when BTIF_HF_SCO_CODEC_MSBC,
                          set 120 when BTIF_HF_SCO_CODEC_CVSD
                    AncFlag : 0 - anc off
                              1 - anc on
                              2 - talk through
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_deal_Aec(float* out, short* in, short* ref, int buf_len, int ref_len);
/***************************************************************
    FunctionName:   soundplus_deal_Tx
    Purpose:        soundplus algorithm Tx ENC process
    Parameter:      
                    out : mic pcm data out.
                    in : mic pcm data in.
                    fb : fb mic pcm data in.
                    numsamples : must be 240
                    AncFlag : 0 - anc off
                              1 - anc on
                              2 - talk through
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_deal_Tx(short* out, float* in, float* fb, int numsamples, int AncFlag);

/***************************************************************
    FunctionName:   soundplus_deal_Rx
    Purpose:        soundplus algorithm Rx process
    Parameter:      
                    buf : bt Rx pcm data.
                    len : Rx pcm data numsamples,
                          set 240 when BTIF_HF_SCO_CODEC_MSBC,
                          set 120 when BTIF_HF_SCO_CODEC_CVSD
                    AncFlag : 0: anc off; 1:anc on
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_deal_Rx(short *inX, int len, int AncFlag);

/***************************************************************
    FunctionName:   soundplus_init
    Purpose:        soundplus algorithm init
    Parameter:      
                    NrwFlag : narrowband flag. 
                              set 1 when BTIF_HF_SCO_CODEC_CVSD, 
                              set 0 when BTIF_HF_SCO_CODEC_MSBC
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_init(int NrwFlag);

/***************************************************************
    FunctionName:   soundplus_deinit
    Purpose:        soundplus algorithm deinit
    Parameter:      
                    void
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_deinit(void);

/***************************************************************
    FunctionName:   soundplus_deal_BF
    Purpose:        soundplus_deal_BF
    Parameter:      
                    buf :
                    buf :
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_deal_BF(short *buf, int len);


/***************************************************************
    FunctionName:   soundplus_bf_init
    Purpose:        soundplus_bf_init
    Parameter:      
                    NrwFlag :
    Return:         return 0 if sucess,else return other
****************************************************************/
int soundplus_bf_init(int NrwFlag);

#ifdef __cplusplus
}
#endif

#endif
