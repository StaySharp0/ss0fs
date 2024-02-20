/*
 * Safe Logging and Tracing Library
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */

#ifndef _SS0FSD_H
#define _SS0FSD_H

#include <types.h>

extern bool_t is_active;

extern void
init_ctl_ipc(void);

extern void
close_ctl_ipc(void);

#endif /* _SS0FSD_H */