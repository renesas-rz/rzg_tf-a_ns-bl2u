/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <debug.h>
#include <fwu_nvm.h>
#include <status.h>
#include <smc.h>
#include <firmware_image_package.h>
#include <platform.h>
#include <platform_def.h>

/* SMC function IDs for Firmware Update operations. */
#define FWU_SMC_UPDATE_DONE		0x15

extern const char build_message[];
extern const char version_string[];

void ns_bl2u_main(void)
{
	smc_args fwu_params = {0};
	smc_ret_values fwu_result = {0};

	NOTICE("NS_BL2U: %s\n", version_string);
	NOTICE("NS_BL2U: %s\n", build_message);

	platform_setup();

	/* Update the FIP image. */
	if (fwu_update_fip(FIP_BKP_ADDRESS) != STATUS_SUCCESS) {
		ERROR("NS_BL2U: Firmware Image Update Failed\n");
		panic();
	}

	/* Call FWU_SMC_UPDATE_DONE to indicate image update done. */
	INFO("NS_BL2U: Calling FWU_SMC_UPDATE_DONE\n");
	fwu_params.fid = FWU_SMC_UPDATE_DONE;
	fwu_result = plat_smc(&fwu_params);
	ERROR("NS_BL2U: Unexpected return from FWU process (%d)\n",
			(int)fwu_result.ret0);
	panic();
}
