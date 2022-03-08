/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <smc.h>

smc_ret_values asm_plat_smc64(uint32_t fid,
				u_register_t arg0,
				u_register_t arg2,
				u_register_t arg3,
				u_register_t arg4,
				u_register_t arg5,
				u_register_t arg6,
				u_register_t arg7);

smc_ret_values plat_smc(const smc_args *args)
{
	return asm_plat_smc64(args->fid,
				args->arg1,
				args->arg2,
				args->arg3,
				args->arg4,
				args->arg5,
				args->arg6,
				args->arg7);
}
