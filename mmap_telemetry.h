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

/*
 * The following are debug macros used throughout the MMAP telemetry.
 *
 * The DEBUG_LEVEL enables the followings based on its value,
 * when dynamic debug option is disabled.
 *
 * 0 = OFF
 * 1 = ASSERTS / ERRORS
 * 2 = 1 + WARN
 * 3 = 2 + INFO
 * 4 = 3 + TRACE
 */

#if (DEBUG_LEVEL < 1)
#define DEBUG_ASSERT(s, ...)
#define DEBUG_ERROR(s, ...)
#else
#define DEBUG_ASSERT(c, s, ...) if (!(c)) { pr_emerg("ASSERT: %s:%d:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__); BUG(); }
#define DEBUG_ERROR(s, ...) pr_err("%s:%d:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#if defined(CONFIG_DYNAMIC_DEBUG)
/*
 * Compile messages for dynamic enable/disable
 */
#define DEBUG_WARN(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DEBUG_INFO(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DEBUG_TRACE(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else

/*
 * Statically compile messages at different levels
 */
#if (DEBUG_LEVEL < 2)
#define DEBUG_WARN(s, ...)
#else
#define DEBUG_WARN(s, ...) pr_warn("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#if (DEBUG_LEVEL < 3)
#define DEBUG_INFO(s, ...)
#else
#define DEBUG_INFO(s, ...) pr_notice("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#if (DEBUG_LEVEL < 4)
#define DEBUG_TRACE(s, ...)
#else
#define DEBUG_TRACE(s, ...) pr_info("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif
#endif

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
