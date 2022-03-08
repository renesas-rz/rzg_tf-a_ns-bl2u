/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FWU_NVM_H__
#define __FWU_NVM_H__

#include <stdint.h>
#include <platform_def.h>
#include <status.h>

/*
 * This function is used to replace the original fip.bin
 * by the backup fip.bin passed through fip_addr argument.
 */
STATUS fwu_update_fip(uintptr_t fip_addr);

#endif /* __FWU_NVM_H__ */
