#include <debug.h>

#include <linux/module.h>

static struct ss0_fs_client*
create_fs_client(void)
{

  create_client();

  return NULL;
}

static int __init
init_ss0fs(void)
{
  dout("hello\n");

  create_fs_client();

  return 0;
}

static void __exit
exit_ss0fs(void)
{
  dout("bye\n");
}

module_init(init_ss0fs);
module_exit(exit_ss0fs);

MODULE_AUTHOR("YongJun Kim <staysharp0@gmail.com>");
MODULE_DESCRIPTION("SS0 filesystem for Linux");
MODULE_LICENSE("GPL");
