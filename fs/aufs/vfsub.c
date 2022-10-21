// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005-2022 Junjiro R. Okajima
 */

/*
 * sub-routines for VFS
 */

#include <linux/namei.h>
#include <linux/cred.h>
#include <linux/security.h>
#include "aufs.h"

struct file *vfsub_dentry_open(struct path *path, int flags)
{
	return dentry_open(path, flags /* | __FMODE_NONOTIFY */,
			   current_cred());
}

struct file *vfsub_filp_open(const char *path, int oflags, int mode)
{
	struct file *file;

	lockdep_off();
	file = filp_open(path,
			 oflags /* | __FMODE_NONOTIFY */,
			 mode);
	lockdep_on();

	return file;
}

int vfsub_kern_path(const char *name, unsigned int flags, struct path *path)
{
	int err;

	err = kern_path(name, flags, path);
	/* add more later */
	return err;
}

struct dentry *vfsub_lookup_one_len(const char *name, struct path *ppath,
				    int len)
{
	struct path path;

	/* VFS checks it too, but by WARN_ON_ONCE() */
	IMustLock(d_inode(ppath->dentry));

	path.dentry = lookup_one_len(name, ppath->dentry, len);
	if (IS_ERR(path.dentry))
		goto out;

out:
	AuTraceErrPtr(path.dentry);
	return path.dentry;
}

/* ---------------------------------------------------------------------- */

/* todo: support mmap_sem? */
ssize_t vfsub_read_u(struct file *file, char __user *ubuf, size_t count,
		     loff_t *ppos)
{
	ssize_t err;

	lockdep_off();
	err = vfs_read(file, ubuf, count, ppos);
	lockdep_on();
	/* re-commit later */
	AuTraceErr(err);
	return err;
}

ssize_t vfsub_read_k(struct file *file, void *kbuf, size_t count,
		     loff_t *ppos)
{
	ssize_t err;

	lockdep_off();
	err = kernel_read(file, kbuf, count, ppos);
	lockdep_on();
	/* re-commit later */
	AuTraceErr(err);
	return err;
}

ssize_t vfsub_write_u(struct file *file, const char __user *ubuf, size_t count,
		      loff_t *ppos)
{
	ssize_t err;

	lockdep_off();
	err = vfs_write(file, ubuf, count, ppos);
	lockdep_on();
	/* re-commit later */
	return err;
}

ssize_t vfsub_write_k(struct file *file, void *kbuf, size_t count, loff_t *ppos)
{
	ssize_t err;

	lockdep_off();
	err = kernel_write(file, kbuf, count, ppos);
	lockdep_on();
	/* re-commit later */
	return err;
}

/* ---------------------------------------------------------------------- */

struct unlink_args {
	int *errp;
	struct inode *dir;
	struct path *path;
	struct inode **delegated_inode;
};

static void call_unlink(void *args)
{
	struct unlink_args *a = args;
	struct dentry *d = a->path->dentry;
	struct inode *h_inode;
	struct user_namespace *userns;
	const int stop_sillyrename = (au_test_nfs(d->d_sb)
				      && au_dcount(d) == 1);

	IMustLock(a->dir);

	a->path->dentry = d->d_parent;
	*a->errp = security_path_unlink(a->path, d);
	a->path->dentry = d;
	if (unlikely(*a->errp))
		return;

	if (!stop_sillyrename)
		dget(d);
	h_inode = NULL;
	if (d_is_positive(d)) {
		h_inode = d_inode(d);
		ihold(h_inode);
	}

	userns = mnt_user_ns(a->path->mnt);
	lockdep_off();
	*a->errp = vfs_unlink(userns, a->dir, d, a->delegated_inode);
	lockdep_on();

	if (!stop_sillyrename)
		dput(d);
	if (h_inode)
		iput(h_inode);

	AuTraceErr(*a->errp);
}

/*
 * @dir: must be locked.
 * @dentry: target dentry.
 */
int vfsub_unlink(struct inode *dir, struct path *path,
		 struct inode **delegated_inode, int force)
{
	int err;
	struct unlink_args args = {
		.errp			= &err,
		.dir			= dir,
		.path			= path,
		.delegated_inode	= delegated_inode
	};

	if (!force)
		call_unlink(&args);
	else {
		int wkq_err;

		wkq_err = au_wkq_wait(call_unlink, &args);
		if (unlikely(wkq_err))
			err = wkq_err;
	}

	return err;
}
