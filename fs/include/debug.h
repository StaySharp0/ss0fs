#ifndef _SS0_DEBUG_H
#define _SS0_DEBUG_H

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define dout(fmt, ...) pr_debug("" fmt, ##__VA_ARGS__)

#endif // !_SS0_DEBUG_H
