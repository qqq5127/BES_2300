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


const U8 CN_BATTERY_LOW_16K[] = { 
	#include "./res/cn/battery_low_16k.txt" 
};

const U32 CN_BATTERY_LOW_16K_SIZE = sizeof(CN_BATTERY_LOW_16K)/sizeof(CN_BATTERY_LOW_16K[0]);


const U8 CN_CONNECTED_16K[] = { 
	#include "./res/cn/connected_16k.txt" 
};

const U32 CN_CONNECTED_16K_SIZE = sizeof(CN_CONNECTED_16K)/sizeof(CN_CONNECTED_16K[0]);


const U8 CN_DISCONNECTED_16K[] = { 
	#include "./res/cn/disconnected_16k.txt" 
};

const U32 CN_DISCONNECTED_16K_SIZE = sizeof(CN_DISCONNECTED_16K)/sizeof(CN_DISCONNECTED_16K[0]);


const U8 CN_HEARTHROUGH_16K[] = { 
	#include "./res/cn/hearthrough_16k.txt" 
};

const U32 CN_HEARTHROUGH_16K_SIZE = sizeof(CN_HEARTHROUGH_16K)/sizeof(CN_HEARTHROUGH_16K[0]);


const U8 CN_POWER_OFF_16K[] = { 
	#include "./res/cn/power_off_16k.txt" 
};

const U32 CN_POWER_OFF_16K_SIZE = sizeof(CN_POWER_OFF_16K)/sizeof(CN_POWER_OFF_16K[0]);


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
