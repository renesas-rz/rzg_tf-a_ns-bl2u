/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <uuid_utils.h>

/* Format string to print a UUID */
static const char *uuid_str_fmt = "{ 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, "
	"0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, "
	"0x%02x, 0x%02x }";

unsigned int is_uuid_null(const uuid_t *uuid)
{
	const uuid_t uuid_null = {0};

	return memcmp(uuid, &uuid_null, sizeof(uuid_t)) == 0;
}

char *uuid_to_str(const uuid_t *uuid, char *str)
{
	assert(uuid != NULL);
	assert(str != NULL);

	snprintf(str, UUID_STR_SIZE, uuid_str_fmt,
		 uuid->time_low[0], uuid->time_low[1],
		 uuid->time_low[2], uuid->time_low[3],
		 uuid->time_mid[0], uuid->time_mid[1],
		 uuid->time_hi_and_version[0], uuid->time_hi_and_version[1],
		 uuid->clock_seq_hi_and_reserved, uuid->clock_seq_low,
		 uuid->node[0], uuid->node[1], uuid->node[2], uuid->node[3],
		 uuid->node[4], uuid->node[5]);

	return str;
}

unsigned int uuid_equal(const uuid_t *uuid1, const uuid_t *uuid2)
{
	return memcmp(uuid1, uuid2, sizeof(uuid_t)) == 0;
}

uuid_t *make_uuid_from_4words(uuid_t *uuid,
			      uint32_t w0,
			      uint32_t w1,
			      uint32_t w2,
			      uint32_t w3)
{
	uint32_t *uuid32;

	assert(uuid != NULL);

	uuid32 = (uint32_t *) uuid;
	uuid32[0] = w0;
	uuid32[1] = w1;
	uuid32[2] = w2;
	uuid32[3] = w3;

	return uuid;
}
