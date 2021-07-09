#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "cqueue.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "spp_api.h"
#include "app_spp.h"
#include "app_utils.h"
#include "aud_section.h"

#if defined(IBRT)
#include "app_tws_ibrt.h"
#endif
#include "app_spp_anc.h"
#include "app_anc.h"
#include "tgt_hardware.h"
#include "app_scpi_test.h"

extern "C" void Scpi_test_init(void);

int app_user_spp_scpi_init(void)
{
	//open spp port
	app_user_spp_server_init();
	Scpi_test_init();
	
	return 0;
}

