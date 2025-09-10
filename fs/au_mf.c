
#include <linux/fs.h>
#include <linux/module.h>

#if IS_MODULE(CONFIG_AUFS_FS)
/*
 * This au_mf.c (aufs mmapped files) is for external AUFS module only,
 * and statically linked to kernel.
 * See also $srctree/fs/Makefile.
 * This linking may seem to be tricky. It is due to support the case when AUFS
 * is configured as an external module, eg. VFS calls a function defined in the
 * dynamic module.
 */
#else
BUILD_BUG_ON_MSG(1, "CONFIG_AUFS_FS is disabled.");
#endif

const struct path *au_file_user_path(struct file *f)
{
	const struct path *path,
		*(*func)(struct file *f),
		*au_do_file_user_path(struct file *f);

	path = NULL;
	func = symbol_get(au_do_file_user_path);
	if (unlikely(!func))
		/* "aufs" module is not loaded */
		goto out;

	path = func(f);
	symbol_put(au_do_file_user_path);

out:
	return path;
}
EXPORT_SYMBOL_GPL(au_file_user_path);

const struct inode *au_file_user_inode(struct file *f)
{
	const struct inode *inode,
		*(*func)(struct file *f),
		*au_do_file_user_inode(struct file *f);

	inode = NULL;
	func = symbol_get(au_do_file_user_inode);
	if (unlikely(!func))
		/* "aufs" module is not loaded */
		goto out;

	inode = func(f);
	symbol_put(au_do_file_user_inode);

out:
	return inode;
}
EXPORT_SYMBOL_GPL(au_file_user_inode);
