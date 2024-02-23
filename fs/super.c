/*
 * SS0 filesystem module
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <linux/fs.h>
#include <linux/module.h>

#include "ss0fs.h"

void
ss0fs_kill_sb(struct super_block* sb)
{
  printk(KERN_INFO "kill_sb\n");
  /* TODO */
}

static void
fill_super(struct super_block* sb, struct ss0fs_context* ctx)
{
  sb->s_blocksize_bits = 0;
  sb->s_blocksize = 4096;
  sb->s_time_gran = 1;
  sb->s_time_min = S64_MIN;
  sb->s_time_max = S64_MAX;

  sb->s_magic = SS0FS_SUPER_MAGIC;
  snprintf(
    sb->s_id, sizeof(sb->s_id), "%u:%u", MAJOR(sb->s_dev), MINOR(sb->s_dev));

  sb->s_maxbytes = MAX_LFS_FILESIZE;
}

int
ss0fs_get_tree_common(struct fs_context* fc)
{
  struct ss0fs_context* ctx = FC2CTX(fc);
  struct super_block* sb;

  fill_super(sb, ctx);

  sb->s_flags |= SB_ACTIVE;

  return 0;
}

static int __init
init_ss0fs(void)
{
  int ret;

  printk(KERN_INFO "hello ss0fs\n");

  if ((ret = register_filesystem(&ss0fs_type)))
    goto out;

  return 0;

out:
  return ret;
}

static void __exit
exit_ss0fs(void)
{
  printk(KERN_INFO "bye ss0fs\n");

  unregister_filesystem(&ss0fs_type);
}

module_init(init_ss0fs);
module_exit(exit_ss0fs);

MODULE_AUTHOR("Yongjun Kim <staysharp0@gmail.com>");
MODULE_DESCRIPTION("SS0 filesystem for Linux");
MODULE_LICENSE("GPL");
