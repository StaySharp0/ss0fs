#ifndef __MOUNT_H__
#define __MOUNT_H__

struct ustr
{
  unsigned int len;
  const char __user* data;
};

struct nfs_mount_data
{
  int version;
  int flags;

  struct ustr mnt_path;
};

#endif /* __MOUNT_H__ */
