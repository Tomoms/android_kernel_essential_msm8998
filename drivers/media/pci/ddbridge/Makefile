#
# Makefile for the ddbridge device driver
#

ddbridge-objs := ddbridge-core.o

obj-$(CONFIG_DVB_DDBRIDGE) += ddbridge.o

ccflags-y += -I $(srctree)/drivers/media/dvb-core/
ccflags-y += -I $(srctree)/drivers/media/dvb-frontends/
ccflags-y += -I $(srctree)/drivers/media/tuners/

# For the staging CI driver cxd2099
ccflags-y += -I $(srctree)/drivers/staging/media/cxd2099/
