#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "log.h"
#include "util.h"

static int ipc_fd = 0;
static int lock_fd = 0;

static int
check_lock_file(char* lock_path)
{
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  if ((lock_fd = open(lock_path, O_WRONLY | O_CREAT, mode)) < 0) {
    eprintf("failed to open lock file for control IPC\n");
    goto out;
  }

  if (lockf(lock_fd, F_TLOCK, 1) < 0) {
    if (errno == EACCES || errno == EAGAIN)
      eprintf("another instance is using %s\n", lock_path);
    else
      eprintf_errno("unable to get lock of control IPC: %s\n", lock_path);
    goto out;
  }

  return 0;
out:
  if (lock_fd > 0)
    close(lock_fd);

  return -1;
}

static int
set_ipc(char* ipc_path)
{
  struct sockaddr_un addr;
  int fd = 0;

  if (strlen(ipc_path) > SIZE_STR_BUF(addr.sun_path)) {
    eprintf("control path too long: %s", ipc_path);
    goto out;
  }

  if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    eprintf_errno("can't open a socket");
    goto out;
  }

  unlink(ipc_path);
  ZEROING(addr);
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, ipc_path);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    eprintf_errno("can't bind a socket");
    goto out;
  }

  if (listen(fd, 32) < 0) {
    eprintf_errno("can't listen a socket");
    goto out;
  }

  /* TODO: event 리팩토링 후 주석 해제
  if (add_event(fd, EPOLLIN, ctl_handler, NULL))
    goto out;
  */

  ipc_fd = fd;

  return 0;
out:
  return -1;
}

void
init_ctl_ipc(void)
{
  extern const char* basename;
  extern short control_port;

  char dir[256], path[256];
  struct stat st;

  snprintf(dir, SIZE_STR_BUF(dir), "/var/run/%s", basename);
  if (stat(dir, &st) < 0)
    mkdir(dir, 0755);

  snprintf_nowarn(
    path, SIZE_STR_BUF(path) - 50, "%s/socket.%d.lock", dir, control_port);
  if (!check_lock_file(path))
    goto out;

  snprintf_nowarn(path, SIZE_STR_BUF(path), "%s/socket.%d", dir, control_port);
  if (!set_ipc(path))
    goto out;

  return;
out:
  if (ipc_fd > 0)
    close(ipc_fd);
  if (lock_fd > 0)
    close(lock_fd);

  exit(1);
}
