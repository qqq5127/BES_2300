#include "gsound_dbg.h"
#include "gsound_target.h"
#include "gsound_custom_tws.h"

void GSoundTargetTwsRoleChangeResponse(bool accepted)
{
    if (accepted)
    {
#if defined(BISTO_USE_TWS_API)
        gsound_tws_on_roleswitch_accepted_by_lib();
#endif
    }
    else
    {
        GLOG_I("tws role switch rejected by gsound!.");
    }
}

void GSoundTargetTwsRoleChangeGSoundDone(void)
{
    GLOG_I("[%s]+++", __func__);

#if defined(BISTO_USE_TWS_API)
    gsound_tws_roleswitch_lib_complete();
#endif

    GLOG_I("[%s]---", __func__);
}

void GSoundTargetTwsInit(const GSoundTwsInterface *handler)
{
    gsound_tws_register_target_handle(handler);

    gsound_tws_init();
}
