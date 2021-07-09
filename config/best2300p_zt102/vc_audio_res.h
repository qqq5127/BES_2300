#ifndef __VC_AUDIO_RES_H__
#define __VC_AUDIO_RES_H__

#if defined(__LANG_CN__)

const U8 CN_ANC_OFF_16K[] = { 
	#include "./res/cn/anc_off_16k.txt" 
};

const U32 CN_ANC_OFF_16K_SIZE = sizeof(CN_ANC_OFF_16K)/sizeof(CN_ANC_OFF_16K[0]);


const U8 CN_ANC_ON_16K[] = { 
	#include "./res/cn/anc_on_16k.txt" 
};

const U32 CN_ANC_ON_16K_SIZE = sizeof(CN_ANC_ON_16K)/sizeof(CN_ANC_ON_16K[0]);


const U8 CN_HEARTHROUGH_16K[] = { 
	#include "./res/cn/hearthrough_16k.txt" 
};

const U32 CN_HEARTHROUGH_16K_SIZE = sizeof(CN_HEARTHROUGH_16K)/sizeof(CN_HEARTHROUGH_16K[0]);


const U8 CN_SOUND_CHARGE_PLEASE[] = { 
	#include "./res/cn/SOUND_CHARGE_PLEASE.txt" 
};

const U32 CN_SOUND_CHARGE_PLEASE_SIZE = sizeof(CN_SOUND_CHARGE_PLEASE)/sizeof(CN_SOUND_CHARGE_PLEASE[0]);


const U8 CN_SOUND_CONNECTED[] = { 
	#include "./res/cn/SOUND_CONNECTED.txt" 
};

const U32 CN_SOUND_CONNECTED_SIZE = sizeof(CN_SOUND_CONNECTED)/sizeof(CN_SOUND_CONNECTED[0]);


const U8 CN_SOUND_DIS_CONNECT[] = { 
	#include "./res/cn/SOUND_DIS_CONNECT.txt" 
};

const U32 CN_SOUND_DIS_CONNECT_SIZE = sizeof(CN_SOUND_DIS_CONNECT)/sizeof(CN_SOUND_DIS_CONNECT[0]);


const U8 CN_SOUND_INCOMING_CALL[] = { 
	#include "./res/cn/SOUND_INCOMING_CALL.txt" 
};

const U32 CN_SOUND_INCOMING_CALL_SIZE = sizeof(CN_SOUND_INCOMING_CALL)/sizeof(CN_SOUND_INCOMING_CALL[0]);


const U8 CN_SOUND_POWER_OFF[] = { 
	#include "./res/cn/SOUND_POWER_OFF.txt" 
};

const U32 CN_SOUND_POWER_OFF_SIZE = sizeof(CN_SOUND_POWER_OFF)/sizeof(CN_SOUND_POWER_OFF[0]);


#endif



#if defined(__LANG_EN__)

#endif



#if defined(__LANG_RING__)

const U8 RING_SOUND_RING_16000[] = { 
	#include "./res/ring/SOUND_RING_16000.txt" 
};

const U32 RING_SOUND_RING_16000_SIZE = sizeof(RING_SOUND_RING_16000)/sizeof(RING_SOUND_RING_16000[0]);


const U8 RING_SOUND_RING_44100[] = { 
	#include "./res/ring/SOUND_RING_44100.txt" 
};

const U32 RING_SOUND_RING_44100_SIZE = sizeof(RING_SOUND_RING_44100)/sizeof(RING_SOUND_RING_44100[0]);


const U8 RING_SOUND_RING_48000[] = { 
	#include "./res/ring/SOUND_RING_48000.txt" 
};

const U32 RING_SOUND_RING_48000_SIZE = sizeof(RING_SOUND_RING_48000)/sizeof(RING_SOUND_RING_48000[0]);


const U8 RING_SOUND_RING_8000[] = { 
	#include "./res/ring/SOUND_RING_8000.txt" 
};

const U32 RING_SOUND_RING_8000_SIZE = sizeof(RING_SOUND_RING_8000)/sizeof(RING_SOUND_RING_8000[0]);


#endif


const U8 RES_SOUND_MUTE[] = { 
	#include "./res/SOUND_MUTE.txt" 
};

const U32 RES_SOUND_MUTE_SIZE = sizeof(RES_SOUND_MUTE)/sizeof(RES_SOUND_MUTE[0]);



#endif	/* __VC_AUDIO_RES_H__ */
