/*
 * SS0 filesystem context
 *
 * Copyright (C) 2024 StaySharp0 Yongjun Kim <staysharp0@gmail.com>
 */
#include <linux/fs_parser.h>
#include <linux/module.h>

#include "ss0fs.h"

static void
fs_context_free(struct fs_context* fc)
{
  struct ss0fs_context* ctx = FC2CTX(fc);

  if (ctx) {
    /* TODO */

    kfree(ctx);
  }
}

static int
fs_context_dup(struct fs_context* fc, struct fs_context* src_fc)
{
  struct ss0fs_context *src = FC2CTX(src_fc), *ctx;

  ctx = kmemdup(src, sizeof(struct ss0fs_context), GFP_KERNEL);
  if (!ctx)
    return -ENOMEM;

  /* TODO */

  fc->fs_private = ctx;
  return 0;
}

static int
fs_context_parse_param(struct fs_context* fc, struct fs_parameter* param)
{
  int opt;
  struct fs_parse_result result;

  if ((opt = fs_parse(fc, ss0fs_params, param, &result) < 0))
    return opt == -ENOPARAM ? 1 : opt;

  /* TODO */

  return 0;
}

static int
fs_context_parse_monolithic(struct fs_context* fc,
                            struct ss0fs_mount_data* data)
{
  struct ss0fs_context* ctx = FC2CTX(fc);

  ctx->version = 0;

  return generic_parse_monolithic(fc, data);
}

/*
 * Create an SS0FS superblock.
 */
static int
fs_context_get_tree(struct fs_context* fc)
{
  /* TODO */

  return ss0fs_get_tree_common(fc);
}

static const struct fs_context_operations ss0fs_cxt_ops = {
  .free = fs_context_free,
  .dup = fs_context_dup,
  .parse_param = fs_context_parse_param,
  .parse_monolithic = fs_context_parse_monolithic,
  .get_tree = fs_context_get_tree,
  .reconfigure = nfs_reconfigure,
};

static int
ss0fs_init_fs_context(struct fs_context* fc)
{
  struct ss0fs_context* ctx;

  ctx = kzalloc(sizeof(struct ss0fs_context), GFP_KERNEL);
  if (unlikely(!ctx))
    return -ENOMEM;

  fc->fs_private = ctx;

  return 0;
}

static const struct fs_parameter_spec ss0fs_params[] = { {} };

struct file_system_type ss0fs_type = {
  .owner = THIS_MODULE,
  .name = "ss0fs",
  .init_fs_context = ss0fs_init_fs_context,
  .parameters = ss0fs_params,
  .kill_sb = ss0fs_kill_sb,
  .fs_flags = FS_RENAME_DOES_D_MOVE | FS_BINARY_MOUNTDATA,
  // FS_ALLOW_IDMAP
};
MODULE_ALIAS_FS("ss0fs");
