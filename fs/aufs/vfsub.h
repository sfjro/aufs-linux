/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2005-2022 Junjiro R. Okajima
 */

/*
 * sub-routines for VFS
 */

#ifndef __AUFS_VFSUB_H__
#define __AUFS_VFSUB_H__

#ifdef __KERNEL__

#include <linux/fs.h>
#include "debug.h"

/* ---------------------------------------------------------------------- */

/* lock subclass for lower inode */
/* default MAX_LOCKDEP_SUBCLASSES(8) is not enough */
/* reduce? gave up. */
enum {
	AuLsc_I_Begin = I_MUTEX_PARENT2, /* 5 */
	AuLsc_I_PARENT,		/* lower inode, parent first */
	AuLsc_I_PARENT2,	/* copyup dirs */
	AuLsc_I_PARENT3,	/* copyup wh */
	AuLsc_I_CHILD,
	AuLsc_I_CHILD2,
	AuLsc_I_End
};

/* to debug easier, do not make them inlined functions */
#define MtxMustLock(mtx)	AuDebugOn(!mutex_is_locked(mtx))
#define IMustLock(i)		AuDebugOn(!inode_is_locked(i))

/* ---------------------------------------------------------------------- */

struct file *vfsub_dentry_open(struct path *path, int flags);
struct file *vfsub_filp_open(const char *path, int oflags, int mode);
int vfsub_kern_path(const char *name, unsigned int flags, struct path *path);
struct dentry *vfsub_lookup_one_len(const char *name, struct path *ppath,
				    int len);

static inline struct dentry *vfsub_lkup_one(struct qstr *name,
					    struct path *ppath)
{
	return vfsub_lookup_one_len(name->name, ppath, name->len);
}

/* ---------------------------------------------------------------------- */

int vfsub_create(struct inode *dir, struct path *path, int mode,
		 bool want_excl);
int vfsub_link(struct dentry *src_dentry, struct inode *dir,
	       struct path *path, struct inode **delegated_inode);

/* ---------------------------------------------------------------------- */

ssize_t vfsub_read_u(struct file *file, char __user *ubuf, size_t count,
		     loff_t *ppos);
ssize_t vfsub_read_k(struct file *file, void *kbuf, size_t count,
			loff_t *ppos);
ssize_t vfsub_write_u(struct file *file, const char __user *ubuf, size_t count,
		      loff_t *ppos);
ssize_t vfsub_write_k(struct file *file, void *kbuf, size_t count,
		      loff_t *ppos);

static inline loff_t vfsub_f_size_read(struct file *file)
{
	return i_size_read(file_inode(file));
}

/* ---------------------------------------------------------------------- */

int vfsub_unlink(struct inode *dir, struct path *path,
		 struct inode **delegated_inode, int force);

#endif /* __KERNEL__ */
#endif /* __AUFS_VFSUB_H__ */
