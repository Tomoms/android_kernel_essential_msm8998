# Makefile for KVM support for MIPS
#

common-objs-y = $(addprefix ../../../virt/kvm/, kvm_main.o coalesced_mmio.o)

EXTRA_CFLAGS += -I $(srctree)/virt/kvm -I $(srctree)/arch/mips/kvm

common-objs-$(CONFIG_CPU_HAS_MSA) += msa.o

kvm-objs := $(common-objs-y) mips.o emulate.o locore.o \
	    interrupt.o stats.o commpage.o \
	    dyntrans.o trap_emul.o fpu.o

obj-$(CONFIG_KVM)	+= kvm.o
obj-y			+= callback.o tlb.o
