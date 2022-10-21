// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005-2022 Junjiro R. Okajima
 */

/*
 * sub-routines for VFS
 */

#include <linux/namei.h>
#include "aufs.h"

int vfsub_kern_path(const char *name, unsigned int flags, struct path *path)
{
	int err;

	err = kern_path(name, flags, path);
	/* add more later */
	return err;
}
