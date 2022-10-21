/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2005-2022 Junjiro R. Okajima
 */

/*
 * debug print functions
 */

#ifndef __AUFS_DEBUG_H__
#define __AUFS_DEBUG_H__

#ifdef __KERNEL__

#ifdef CONFIG_AUFS_DEBUG
#define AuDebugOn(a)		BUG_ON(a)
#else
#define AuDebugOn(a)		do {} while (0)
#endif /* CONFIG_AUFS_DEBUG */

#endif /* __KERNEL__ */
#endif /* __AUFS_DEBUG_H__ */
