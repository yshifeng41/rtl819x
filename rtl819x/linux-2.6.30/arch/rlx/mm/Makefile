#
# Makefile for the Linux/MIPS-specific parts of the memory manager.
#

obj-y  += cache.o dma-default.o extable.o fault.o \
          init.o tlbex.o tlbex-fault.o uasm.o page-rlx.o

obj-y  += ioremap.o pgtable-32.o cache-rlx.o tlb-rlx.o

ifneq ($(CONFIG_RTL_8198B),y)
obj-y  += imem-dmem.o
endif

obj-$(CONFIG_HIGHMEM)		+= highmem.o

EXTRA_CFLAGS += -Werror
