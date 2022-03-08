/*
 * Copyright (c) 2015-2019, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2019, Linaro Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

void waitus(uint64_t us);
void waitms(uint64_t ms);

#endif /* DELAY_H */
