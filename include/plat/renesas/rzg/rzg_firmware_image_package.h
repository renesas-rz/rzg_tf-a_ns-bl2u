/*
 * Copyright (c) 2014-2018, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RZG_FIRMWARE_IMAGE_PACKAGE_H
#define RZG_FIRMWARE_IMAGE_PACKAGE_H

#include <stdint.h>

#include "uuid.h"


#define TOC_HEADER_NAME_FW_IPL	0xAA640001
#define TOC_HEADER_NAME_KEYRING	0xAA640002
#define TOC_HEADER_NAME_BOOT_FW	0xAA640003
#define TOC_HEADER_NAME_NS_BL2U	0xAA640004
#define TOC_HEADER_NAME_EMPTY	0xFFFFFFFF


/* ToC Entry UUIDs */
#define UUID_TRUSTED_UPDATE_FIRMWARE_NS_BL2U \
	{{0x4f, 0x51, 0x1d, 0x11}, {0x2b, 0xe5}, {0x4e, 0x49}, 0xb4, 0xc5, {0x83, 0xc2, 0xf7, 0x15, 0x84, 0x0a} }
#define UUID_TRUSTED_BOOT_FIRMWARE_BL2 \
	{{0x5f, 0xf9, 0xec, 0x0b}, {0x4d, 0x22}, {0x3e, 0x4d}, 0xa5, 0x44, {0xc3, 0x9d, 0x81, 0xc7, 0x3f, 0x0a} }
#define UUID_EL3_RUNTIME_FIRMWARE_BL31 \
	{{0x47, 0xd4, 0x08, 0x6d}, {0x4c, 0xfe}, {0x98, 0x46}, 0x9b, 0x95, {0x29, 0x50, 0xcb, 0xbd, 0x5a, 0x00} }
#define UUID_SECURE_PAYLOAD_BL32 \
	{{0x05, 0xd0, 0xe1, 0x89}, {0x53, 0xdc}, {0x13, 0x47}, 0x8d, 0x2b, {0x50, 0x0a, 0x4b, 0x7a, 0x3e, 0x38} }
#define UUID_SECURE_PAYLOAD_BL32_EXTRA2 \
	{{0xa7, 0x4d, 0x09, 0x44}, {0x4f, 0x2c}, {0x11, 0xeb}, 0xa7, 0x2d, {0xf3, 0x35, 0x07, 0xaf, 0x39, 0x9a} }
#define UUID_SECURE_PAYLOAD_BL32_EXTRA3 \
	{{0xab, 0x46, 0x5c, 0x30}, {0x4f, 0x2c}, {0x11, 0xeb}, 0xa1, 0x7c, {0xbf, 0x2d, 0x2a, 0x50, 0x23, 0x77} }
#define UUID_SECURE_PAYLOAD_BL32_EXTRA4 \
	{{0xab, 0xb9, 0xd4, 0xda}, {0x4f, 0x2c}, {0x11, 0xeb}, 0xbd, 0xf3, {0xb3, 0x59, 0x22, 0x6b, 0xdc, 0x5a} }
#define UUID_SECURE_PAYLOAD_BL32_EXTRA5 \
	{{0xac, 0x02, 0x29, 0x9c}, {0x4f, 0x2c}, {0x11, 0xeb}, 0x9d, 0x8c, {0x0f, 0xc9, 0xeb, 0x8a, 0x1f, 0xd2} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33 \
	{{0xd6, 0xd0, 0xee, 0xa7}, {0xfc, 0xea}, {0xd5, 0x4b}, 0x97, 0x82, {0x99, 0x34, 0xf2, 0x34, 0xb6, 0xe4} }
#define UUID_SECURE_PAYLOAD_BL32_EXTRA1 \
	{{0xa4, 0x9c, 0xa1, 0xd2}, {0x4f, 0x2c}, {0x11, 0xeb}, 0xa3, 0xe0, {0xe7, 0xd3, 0xf6, 0x51, 0x61, 0x52} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA1 \
	{{0xd6, 0x46, 0xf9, 0xfc}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xbd, 0x4c, {0x6f, 0xeb, 0x6b, 0x96, 0x6b, 0xca} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA2 \
	{{0xd6, 0xce, 0x07, 0xc6}, {0x4f, 0x2d}, {0x11, 0xeb}, 0x9f, 0xf1, {0x4b, 0xfa, 0x50, 0xb4, 0x03, 0x41} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA3 \
	{{0xd7, 0x24, 0xfb, 0xe4}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xb8, 0x15, {0xcb, 0x5e, 0xbe, 0x25, 0xc2, 0xc4} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA4 \
	{{0xd7, 0x90, 0x17, 0x76}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xb3, 0x0c, {0xdf, 0x43, 0x89, 0x23, 0x0d, 0x61} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA5 \
	{{0xd7, 0xe7, 0x20, 0x66}, {0x4f, 0x2d}, {0x11, 0xeb}, 0x98, 0x54, {0x8f, 0x01, 0x5f, 0x81, 0xe9, 0x7c} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA6 \
	{{0xd8, 0x38, 0xfd, 0xaa}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xaa, 0x5b, {0x93, 0x2a, 0x77, 0xf6, 0x9f, 0x4c} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA7 \
	{{0xd8, 0x98, 0x76, 0x0e}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xa5, 0x41, {0xc7, 0x2b, 0xf3, 0x39, 0xd6, 0x50} }
#define UUID_NON_TRUSTED_FIRMWARE_BL33_EXTRA8 \
	{{0xd9, 0x02, 0xbd, 0xd4}, {0x4f, 0x2d}, {0x11, 0xeb}, 0xb1, 0x67, {0xa3, 0x67, 0x20, 0xf6, 0xeb, 0x3a} }
/* Content certificates */
#define UUID_TRUSTED_BOOT_FW_CERT \
	{{0xd6, 0xe2, 0x69, 0xea}, {0x5d, 0x63}, {0xe4, 0x11}, 0x8d, 0x8c, {0x9f, 0xba, 0xbe, 0x99, 0x56, 0xa5} }
#define UUID_SOC_FW_CONTENT_CERT \
	{{0xe2, 0xb2, 0x0c, 0x20}, {0x5e, 0x63}, {0xe4, 0x11}, 0x9c, 0xe8, {0xab, 0xcc, 0xf9, 0x2b, 0xb6, 0x66} }
/* Trusted Boot Keyring */
#define UUID_TRUSTED_BOOT_KEYRING \
	{{0xcf, 0x6d, 0xf0, 0xf3}, {0x8f, 0x9e}, {0x43, 0x04}, 0x9e, 0xbd, {0xe1, 0x63, 0x15, 0x69, 0xa4, 0xbb} }
/* Security Module */
#define UUID_TRUSTED_BOOT_SEC_MODULE \
	{{0x86, 0x7e, 0xcf, 0xd1}, {0xd8, 0x26}, {0x42, 0x93}, 0xb2, 0xe3, {0x9a, 0x1a, 0x5a, 0x77, 0x4c, 0x11} }

typedef struct fip_toc_header {
	uint32_t	name;
	uint32_t	serial_number;
	uint64_t	flags;
} fip_toc_header_t;

typedef struct fip_toc_entry {
	uuid_t		uuid;
	uint64_t	offset_address;
	uint64_t	size;
	uint64_t	flags;
	uint64_t	nvm_offset;
} fip_toc_entry_t;

#endif /* RZG_FIRMWARE_IMAGE_PACKAGE_H */

