/*
 *  ======= DAC ========
 *  DAC target-side implementation
 *
 *  Created on: Oct 18, 2019
 *  Author:     victor
 */
#include <xdc/std.h>
#include <xdc/runtime/Startup.h>

#include "DAC.h"

/* include DAC internal implementation definitions */
#include "package/internal/DAC.xdc.h"

/*
 *  ======== DAC_Module_startup ========
 */
Int DAC_Module_startup(Int state)
{
    return (Startup_DONE);
}

#ifdef DAC_Object
/*
 *  ======== DAC_Instance_init ========
 *  DAC created or constructed instance object initialization
 */
Void DAC_Instance_init(DAC_Object *obj, const DAC_Params *params)
{
    /* TODO: initialize DAC instance state fields */
}
#endif
