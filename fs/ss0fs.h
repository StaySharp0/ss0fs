#ifndef __SS0FS_H__
#define __SS0FS_H__

#include <linux/fs_context.h>

#define FC2CTX(fc) (fc->fs_private)

#define SS0FS_SUPER_MAGIC 0xE1E6

struct ss0fs_context
{
  unsigned int version;
  unsigned int minorversion;
};

struct ss0fs_mount_data
{
  int version;
};

/* super.c */
void
ss0fs_kill_sb(struct super_block* sb);
int
ss0fs_get_tree_common(struct fs_context* fc);

/* fs_context.c */
extern struct file_system_type ss0fs_type;

#endif /* __SS0FS_H__ */
