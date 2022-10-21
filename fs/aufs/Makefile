# SPDX-License-Identifier: GPL-2.0

-include ${srctree}/${src}/priv_def.mk

# sparse requires the full pathname
ccflags-y += -include ${srctree}/include/uapi/linux/aufs_type.h

obj-$(CONFIG_AUFS_FS) += aufs.o
aufs-y := module.o super.o \
	iinfo.o
# add more later
