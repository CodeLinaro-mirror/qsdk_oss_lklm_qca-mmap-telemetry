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

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>
#include <linux/mm.h>

#include "mmap_telemetry.h"

static struct mmap_telemetry_uio_info info_global[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX];
static struct device *dev_global[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX];
static void *mem[MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX][MAX_UIO_MAPS] = {0};

/*
 * mmap_telemetry_release
 *	This is to release the device
 */
static void mmap_telemetry_dev_release(struct device *dev)
{
	return;
}

/*
 * mmap_telemetry_uio_open
 *	This is to open the uio device
 */
static int mmap_telemetry_uio_open(struct uio_info *info, struct inode *inode)
{
	return 0;
}

/*
 * mmap_telemetry_uio_release
 *	This is to release the uio device
 */
static int mmap_telemetry_uio_release(struct uio_info *info, struct inode *inode)
{
	return 0;
}

/*
 * mmap_telemetry_uio_irq_handler
 *	This is to handle the uio irq.
 */
static irqreturn_t mmap_telemetry_uio_irq_handler(int irq, struct uio_info *info)
{
	pr_info("irq: %d.", irq);

	return IRQ_HANDLED;
}

/*
 * mmap_telemetry_uio_irq_control
 *	This is to control the uio irq.
 */
static int mmap_telemetry_uio_irq_control(struct uio_info *info, s32 irq_on)
{
	return 0;
}

/*
 * mmap_telemetry_mmap
 * 	This is to mmap the uio memory.
 */
static int mmap_telemetry_mmap(struct uio_info *info, struct vm_area_struct *vma)
{
	u32 ret;
	u32 pfn;

	pfn = (info->mem[vma->vm_pgoff].addr) >> PAGE_SHIFT;

	ret = remap_pfn_range(vma, vma->vm_start, pfn,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
	if (ret) {
		/* Error Handle */
		pr_warn("remap_pfn_range failed");
	}

	return ret;
}

/*
 * mmap_telemetry_round_up_to_page_size
 *	Round up the size to next multiple of PAGE_SIZE
 */
static size_t mmap_telemetry_round_up_to_page_size(size_t size)
{
	return (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

/*
 * mmap_telemetry_alloc_mem
 * 	Allocate the mmap memory.
 */
void *mmap_telemetry_alloc_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type, unsigned long map_size, bool is_coherent)
{
	void *memory;
	struct uio_info *info = info_global[dev_type].info;
	int i;
	unsigned long flags;

	if (is_coherent) {
		pr_warn("Not supported. \n");
		return NULL;
	}

	map_size = mmap_telemetry_round_up_to_page_size(map_size);
	memory = kzalloc(map_size, GFP_KERNEL);
	if (!memory) {
		pr_warn("Memory allocation failed for %s telemetry. \n", mmap_telemetry_mem_type_str[dev_type][map_type]);
		return NULL;
	}

	spin_lock_irqsave(&info_global[dev_type].uio_lock, flags);

	if (info->mem[map_type].addr) {
		kfree(phys_to_virt(info->mem[map_type].addr));
	}

	info->mem[map_type].addr = (phys_addr_t)(virt_to_phys(memory));
	info->mem[map_type].size = map_size;
	info->mem[map_type].memtype = UIO_MEM_LOGICAL;
	info->mem[map_type].name = mmap_telemetry_mem_type_str[dev_type][map_type];

	for (i = 0; i < map_type; i++){
		info->mem[map_type].offs += info->mem[i].size;
	}

	spin_unlock_irqrestore(&info_global[dev_type].uio_lock, flags);
        return memory;
}
EXPORT_SYMBOL(mmap_telemetry_alloc_mem);

/*
 * mmap_telemetry_get_mem
 *	Get the mmap memory address.
 */
void *mmap_telemetry_get_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type)
{
	struct uio_info *info = info_global[dev_type].info;
	unsigned long flags;
	void *addr = NULL;

	spin_lock_irqsave(&info_global[dev_type].uio_lock, flags);

	if (info->mem[map_type].name) {
		addr = phys_to_virt(info->mem[map_type].addr);
		spin_unlock_irqrestore(&info_global[dev_type].uio_lock, flags);
		return addr;
	}

	spin_unlock_irqrestore(&info_global[dev_type].uio_lock, flags);
	return NULL;
}
EXPORT_SYMBOL(mmap_telemetry_get_mem);

/*
 * mmap_telemetry_free_mem
 *	Free the mmap memory.
 */
void mmap_telemetry_free_mem(enum mmap_telemetry_uio_dev_category dev_type, int map_type)
{
	struct uio_info *info = info_global[dev_type].info;
	unsigned long flags;

	spin_lock_irqsave(&info_global[dev_type].uio_lock, flags);

	if (info->mem[map_type].name) {
		kfree(phys_to_virt(info->mem[map_type].addr));
		info->mem[map_type].name = NULL;
	}

	spin_unlock_irqrestore(&info_global[dev_type].uio_lock, flags);
}
EXPORT_SYMBOL(mmap_telemetry_free_mem);

/*
 * mmap_telemetry_cleanup_mem
 *	Clean up the uio map memory
 */
static void mmap_telemetry_cleanup_mem(void)
{
	int dev_type, map_type, num_maps;
	unsigned long flags;

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		num_maps = mmap_telemetry_num_maps_per_category[dev_type];
		spin_lock_irqsave(&info_global[dev_type].uio_lock, flags);

		for (map_type = 0; map_type < num_maps; map_type++) {
			if(mem[dev_type][map_type]) {
				kfree(mem[dev_type][map_type]);
			}
		}

		spin_unlock_irqrestore(&info_global[dev_type].uio_lock, flags);
	}
}

/*
 * mmap_telemetry_cleanup_uio
 *	Clean up the uio info structures
 */
static void mmap_telemetry_cleanup_uio(void)
{
	int dev_type;
	struct uio_info *info;

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		info = info_global[dev_type].info;

		if (info) {
			kfree(info);
		}
	}
}

/*
 * mmap_telemetry_cleanup_dev
 *	Clean up the device structures
 */
static void mmap_telemetry_cleanup_dev(void)
{
	int dev_type;
	struct device *dev;

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		dev = dev_global[dev_type];

		if (dev) {
			kfree(dev);
		}
	}
}

/*
 * mmap_telemetry_dev_unregister
 *	Unregister the device structures
 */
static void mmap_telemetry_dev_unregister(enum mmap_telemetry_uio_dev_category dev_type)
{
	int i;
	struct device *dev;

	for (i = 0; i < dev_type; i++) {
		dev = dev_global[i];

		if (dev) {
			device_unregister(dev);
		}
	}
}

/*
 * mmap_telemetry_uio_unregister
 *	Unregister the uio info structures
 */
static void mmap_telemetry_uio_unregister(enum mmap_telemetry_uio_dev_category dev_type)
{
	int i;
	struct uio_info *info;

	for (i = 0; i < dev_type; i++) {
		info = info_global[i].info;

		if (info) {
			uio_unregister_device(info);
		}
	}
}

/*
 * mmap_telemetry_init
 * 	Initialize the mmap telemetry module.
 */
static int __init mmap_telemetry_init(void)
{
	int ret, dev_type, map_type;
	int num_maps;
	struct uio_info *info;
	struct device *dev;
	void *memory;

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		num_maps = mmap_telemetry_num_maps_per_category[dev_type];

		for (map_type = 0; map_type < num_maps; map_type++) {
			memory = kzalloc(PAGE_SIZE, GFP_KERNEL);
			if (!memory) {
				pr_warn("Memory allocation failed for telemetry. \n");
				mmap_telemetry_cleanup_mem();
				return -ENOMEM;
			}

			mem[dev_type][map_type] = memory;
		}
	}

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
		if (!info) {
			pr_warn("Failed to allocate meory for uio info.\n");
			mmap_telemetry_cleanup_uio();
			mmap_telemetry_cleanup_mem();
			return -ENOMEM;
		}

		info->name = mmap_telemetry_uio_dev_category_str[dev_type];
		info->version = "0.0.1";
		info->open = mmap_telemetry_uio_open;
		info->release = mmap_telemetry_uio_release;
		info->irq = UIO_IRQ_CUSTOM;
		info->handler = mmap_telemetry_uio_irq_handler;
		info->irqcontrol = mmap_telemetry_uio_irq_control;
		info->mmap = mmap_telemetry_mmap;

		num_maps = mmap_telemetry_num_maps_per_category[dev_type];

		for (map_type = 0; map_type < num_maps; map_type++) {
			info->mem[map_type].addr = (phys_addr_t)(virt_to_phys(mem[dev_type][map_type]));
			info->mem[map_type].size = PAGE_SIZE;
			info->mem[map_type].memtype = UIO_MEM_LOGICAL;
			info->mem[map_type].name = mmap_telemetry_mem_type_str[dev_type][map_type];
			info->mem[map_type].offs = map_type * PAGE_SIZE;
		}

		info_global[dev_type].info = info;
		spin_lock_init(&info_global[dev_type].uio_lock);
	}

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		dev = kzalloc(sizeof(struct device), GFP_KERNEL);
		if (!dev) {
			mmap_telemetry_cleanup_dev();
			mmap_telemetry_cleanup_uio();
			mmap_telemetry_cleanup_mem();
			return -ENOMEM;
		}

		dev_set_name(dev, mmap_telemetry_uio_dev_category_str[dev_type]);
		dev->release = mmap_telemetry_dev_release;
		dev->parent = NULL;

		dev_global[dev_type] = dev;
	}

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		dev = dev_global[dev_type];

		ret = device_register(dev);
		if (ret) {
			pr_warn("Failed to register the telemetry uio device.\n");
			mmap_telemetry_dev_unregister(dev_type);
			mmap_telemetry_cleanup_dev();
                        mmap_telemetry_cleanup_uio();
                        mmap_telemetry_cleanup_mem();
			return ret;
		}
	}

	for (dev_type = 0; dev_type < MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX; dev_type++) {
		info = info_global[dev_type].info;
		dev = dev_global[dev_type];

		if (uio_register_device(dev, info) < 0) {
			mmap_telemetry_uio_unregister(dev_type);
			mmap_telemetry_dev_unregister(MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX);
			mmap_telemetry_cleanup_dev();
			mmap_telemetry_cleanup_uio();
			mmap_telemetry_cleanup_mem();
			return -ENOMEM;
		}

		pr_info("Registered UIO handler %s\n", mmap_telemetry_uio_dev_category_str[dev_type]);
	}

	return 0;
}

/*
 * mmap_telemetry_exit
 * 	Deinitialize the mmap telemetry module.
 */
static void __exit mmap_telemetry_exit(void)
{
	mmap_telemetry_uio_unregister(MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX);
	mmap_telemetry_dev_unregister(MMAP_TELEMETRY_UIO_DEV_CATEGORY_MAX);
	mmap_telemetry_cleanup_dev();
	mmap_telemetry_cleanup_uio();
	mmap_telemetry_cleanup_mem();
}

module_init(mmap_telemetry_init);
module_exit(mmap_telemetry_exit);

MODULE_DESCRIPTION("MMAP TELEMETRY");
MODULE_LICENSE("Dual BSD/GPL");
