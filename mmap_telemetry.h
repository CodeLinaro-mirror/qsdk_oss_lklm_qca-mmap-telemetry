/*
 **************************************************************************
 * Copyright (c) 2025 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#ifndef __MMAP_TELEMETRY_H_
#define __MMAP_TELEMETRY_H_

#include <linux/uio_driver.h>
#include "mmap_telemetry_public.h"

int mmap_telemetry_num_maps_per_category[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX] = { MMAP_TELEMETRY_MEM_TYPE_NSS_MAX, 1, 1, 1, 1 };

/*
 * mmap_telemetry_uio_dev_category_str
 *	mmap telemetry uio device category strings
 */
const char* mmap_telemetry_uio_dev_category_str[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX] = {
	"nss",
	"wlan_host",
	"wifi_fw",
	"halphy",
	"ucode"
};

/*
 * mmap_telemetry_mem_type_str
 *	mmap telemetry memory type strings
 */
const char* mmap_telemetry_mem_type_str[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX][MAX_UIO_MAPS] = {
	{"sfe", "ecm", "ppe", "stc", "dev clf"},
	{"wlan_host"},
	{"wifi_fw"},
	{"halphy"},
	{"ucode"}
};

/*
 * mmap_telemetry_uio_info
 *	mmap telemetry uio info structure
 */
struct mmap_telemetry_uio_info {
	struct uio_info *info;
	spinlock_t uio_lock;
};

#endif // __MMAP_TELEMETRY_H_
