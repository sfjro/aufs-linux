// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005-2022 Junjiro R. Okajima
 */

/*
 * inode functions
 */

#include <linux/cred.h>
#include "aufs.h"

struct inode *au_igrab(struct inode *inode)
{
	if (inode) {
		AuDebugOn(!atomic_read(&inode->i_count));
		ihold(inode);
	}
	return inode;
}

int au_test_h_perm(struct user_namespace *h_userns, struct inode *h_inode,
		   int mask)
{
	if (uid_eq(current_fsuid(), GLOBAL_ROOT_UID))
		return 0;
	return inode_permission(h_userns, h_inode, mask);
}

int au_test_h_perm_sio(struct user_namespace *h_userns, struct inode *h_inode,
		       int mask)
{
	if (au_test_nfs(h_inode->i_sb)
	    && (mask & MAY_WRITE)
	    && S_ISDIR(h_inode->i_mode))
		mask |= MAY_READ; /* force permission check */
	return au_test_h_perm(h_userns, h_inode, mask);
}
