/*
 * arch/sh/mm/consistent.c
 *
 * Copyright (C) 2004 - 2007  Paul Mundt
 *
 * Declared coherent memory functions based on arch/x86/kernel/pci-dma_32.c
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <asm/addrspace.h>
#include <asm/io.h>

struct dma_coherent_mem {
	void		*virt_base;
	u32		device_base;
	int		size;
	int		flags;
	unsigned long	*bitmap;
};

void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	void *ret, *ret_nocache;
	int order = get_order(size);

	if (dma_alloc_from_coherent(dev, size, dma_handle, &ret))
		return ret;

	ret = (void *)__get_free_pages(gfp, order);
	if (!ret)
		return NULL;

	memset(ret, 0, size);
	/*
	 * Pages from the page allocator may have data present in
	 * cache. So flush the cache before using uncached memory.
	 */
	dma_cache_sync(dev, ret, size, DMA_BIDIRECTIONAL);

	ret_nocache = ioremap_nocache(virt_to_phys(ret), size);
	if (!ret_nocache) {
		free_pages((unsigned long)ret, order);
		return NULL;
	}

	*dma_handle = virt_to_phys(ret);
	return ret_nocache;
}
EXPORT_SYMBOL(dma_alloc_coherent);

void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle)
{
	struct dma_coherent_mem *mem = dev ? dev->dma_mem : NULL;
	int order = get_order(size);

	if (!dma_release_from_coherent(dev, order, vaddr)) {
		WARN_ON(irqs_disabled());	/* for portability */
		BUG_ON(mem && mem->flags & DMA_MEMORY_EXCLUSIVE);
		free_pages((unsigned long)phys_to_virt(dma_handle), order);
		iounmap(vaddr);
	}
}
EXPORT_SYMBOL(dma_free_coherent);

void dma_cache_sync(struct device *dev, void *vaddr, size_t size,
		    enum dma_data_direction direction)
{
#ifdef CONFIG_CPU_SH5
	void *p1addr = vaddr;
#else
	void *p1addr = (void*) P1SEGADDR((unsigned long)vaddr);
#endif

	switch (direction) {
	case DMA_FROM_DEVICE:		/* invalidate only */
		__flush_invalidate_region(p1addr, size);
		break;
	case DMA_TO_DEVICE:		/* writeback only */
		__flush_wback_region(p1addr, size);
		break;
	case DMA_BIDIRECTIONAL:		/* writeback and invalidate */
		__flush_purge_region(p1addr, size);
		break;
	default:
		BUG();
	}
}
EXPORT_SYMBOL(dma_cache_sync);

static int __init memchunk_setup(char *str)
{
	return 1; /* accept anything that begins with "memchunk." */
}
__setup("memchunk.", memchunk_setup);

static void __init memchunk_cmdline_override(char *name, unsigned long *sizep)
{
	char *p = boot_command_line;
	int k = strlen(name);

	while ((p = strstr(p, "memchunk."))) {
		p += 9; /* strlen("memchunk.") */
		if (!strncmp(name, p, k) && p[k] == '=') {
			p += k + 1;
			*sizep = memparse(p, NULL);
			pr_info("%s: forcing memory chunk size to 0x%08lx\n",
				name, *sizep);
			break;
		}
	}
}

int __init platform_resource_setup_memory(struct platform_device *pdev,
					  char *name, unsigned long memsize)
{
	struct resource *r;
	dma_addr_t dma_handle;
	void *buf;

	r = pdev->resource + pdev->num_resources - 1;
	if (r->flags) {
		pr_warning("%s: unable to find empty space for resource\n",
			name);
		return -EINVAL;
	}

	memchunk_cmdline_override(name, &memsize);
	if (!memsize)
		return 0;

	buf = dma_alloc_coherent(NULL, memsize, &dma_handle, GFP_KERNEL);
	if (!buf) {
		pr_warning("%s: unable to allocate memory\n", name);
		return -ENOMEM;
	}

	memset(buf, 0, memsize);

	r->flags = IORESOURCE_MEM;
	r->start = dma_handle;
	r->end = r->start + memsize - 1;
	r->name = name;
	return 0;
}
