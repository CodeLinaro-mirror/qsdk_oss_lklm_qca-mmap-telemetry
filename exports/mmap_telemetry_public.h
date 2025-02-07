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

#ifndef __MMAP_TELEMETRY_PUBLIC_H_
#define __MMAP_TELEMETRY_PUBLIC_H_

/**
 * @file mmap_telemetry_public.h
 *      MMAP telemetry exported function headers for the telemetry framework.
 */

/**
 * Mmap Telemery uio device category enum.
 */
typedef enum /** @cond */ mmap_telemetry_uio_dev_category /** @endcond */{
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_NSS,		/**< UIO device for NSS statistics. */
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_WLAN_HOST,	/**< UIO device for WLAN host statistics. */
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_WIFI_FW,	/**< UIO device for WiFi firmware statistics. */
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_HALPHY,		/**< UIO device for halphy statistics. */
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_UCODE,		/**< UIO device for ucode statistics. */
	MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX		/**< Indicates the last item. */
} mmap_telemetry_uio_dev_category;

/**
 * MMAP Telemetry memory map type enum.
 */
typedef enum  /** @cond */ mmap_telemetry_mem_type_nss /** @endcond */{
	MMAP_TELEMETRY_MEM_TYPE_NSS_SFE,	/**< Memory map for sfe statistics. */
	MMAP_TELEMETRY_MEM_TYPE_NSS_ECM,	/**< Memory map for ecm statistics.*/
	MMAP_TELEMETRY_MEM_TYPE_NSS_PPE,	/**< Memory map for ppe statistics.*/
	MMAP_TELEMETRY_MEM_TYPE_NSS_STC,	/**< Memory map for stc statistics. */
	MMAP_TELEMETRY_MEM_TYPE_NSS_DEV_CLF,	/**< Memory map for device classification statistics.*/
	MMAP_TELEMETRY_MEM_TYPE_NSS_MAX		/**< Indicates the last item. */
} mmap_telemetry_mem_type_nss;

/**
 * Allocate mmap telemetry memory map.
 *
 * @param	dev_type	Indicates UIO device type.
 * @param       map_type	Indicates memory map type.
 * @param       map_size	Size of the memory map.
 * @param	is_coherent	Indicate if memory is coherent.
 *
 * @return
 * Return address of the allocated memory.
 */
void *mmap_telemetry_alloc_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type, unsigned long map_size, bool is_coherent);

/**
 * Get mmap telemetry memory map address.
 *
 * @param	dev_type	Indicates UIO device type.
 * @param       map_type	Indicates memory map type.
 *
 * @return
 * Return address of the allocated memory.
 */
void *mmap_telemetry_get_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type);

/**
 * Free mmap telemetry memory map.
 *
 * @param	dev_type	Indicates UIO device type.
 * @param       map_type	Indicates memory map type.
 *
 * @return
 * None.
 */
void mmap_telemetry_free_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type);

#endif //__MMAP_TELEMETRY_PUBLIC_H_
