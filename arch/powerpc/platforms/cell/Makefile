obj-$(CONFIG_PPC_CELL_NATIVE)		+= interrupt.o iommu.o setup.o \
					   cbe_regs.o spider-pic.o \
					   pervasive.o pmu.o io-workarounds.o \
					   spider-pci.o
obj-$(CONFIG_CBE_RAS)			+= ras.o

obj-$(CONFIG_CBE_THERM)			+= cbe_thermal.o
obj-$(CONFIG_CBE_CPUFREQ_PMI)		+= cbe_cpufreq_pmi.o
obj-$(CONFIG_CBE_CPUFREQ)		+= cbe-cpufreq.o
cbe-cpufreq-y				+= cbe_cpufreq_pervasive.o cbe_cpufreq.o
obj-$(CONFIG_CBE_CPUFREQ_SPU_GOVERNOR)	+= cpufreq_spudemand.o

obj-$(CONFIG_PPC_IBM_CELL_POWERBUTTON)	+= cbe_powerbutton.o

ifeq ($(CONFIG_SMP),y)
obj-$(CONFIG_PPC_CELL_NATIVE)		+= smp.o
endif

# needed only when building loadable spufs.ko
spu-priv1-$(CONFIG_PPC_CELL_NATIVE)	+= spu_priv1_mmio.o

spu-manage-$(CONFIG_PPC_CELLEB)		+= spu_manage.o
spu-manage-$(CONFIG_PPC_CELL_NATIVE)	+= spu_manage.o

obj-$(CONFIG_SPU_BASE)			+= spu_callbacks.o spu_base.o \
					   spu_notify.o \
					   spu_syscalls.o spu_fault.o \
					   $(spu-priv1-y) \
					   $(spu-manage-y) \
					   spufs/

obj-$(CONFIG_PCI_MSI)			+= axon_msi.o


# celleb stuff
ifeq ($(CONFIG_PPC_CELLEB),y)
obj-y					+= celleb_setup.o \
					   celleb_pci.o celleb_scc_epci.o \
					   celleb_scc_pciex.o \
					   celleb_scc_uhc.o \
					   io-workarounds.o spider-pci.o \
					   beat.o beat_htab.o beat_hvCall.o \
					   beat_interrupt.o beat_iommu.o

obj-$(CONFIG_SMP)			+= beat_smp.o
obj-$(CONFIG_PPC_UDBG_BEAT)		+= beat_udbg.o
obj-$(CONFIG_SERIAL_TXX9)		+= celleb_scc_sio.o
obj-$(CONFIG_SPU_BASE)			+= beat_spu_priv1.o
endif
