#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "log.h"
#include "svc.h"
#include "svc_internal.h"
#include "types.h"
#include "util.h"

#if defined(SO_PEERCRED) || defined(SCM_CREDENTIALS)
struct ucred
{
  pid_t pid; /* PID of sending process.  */
  uid_t uid; /* UID of sending process.  */
  gid_t gid; /* GID of sending process.  */
};
#endif

static int ipc_fd = 0;
static int lock_fd = 0;

typedef enum
{
  HDR_RECV,
  PDU_RECV,
  HDR_SEND,
  PDU_SEND,
} cntl_state_t;

typedef struct
{
  int fd;
  cntl_state_t state;
  int progress;
  cntl_req_t req;
  cntl_res_t res;
} cntl_job_t;

static cntl_job_t*
alloc_cntl_job(void)
{
  cntl_job_t* job;

  if (!(job = CALLOC(1, job))) {
    eprintf("can't allocate control job");
    goto out;
  }

  job->state = HDR_RECV;

out:
  dprintf("cntl_job :%p", job);
  return job;
}

static void
free_cntl_job(cntl_job_t* job)
{
  dprintf("cntl_job :%p", job);

  free(job);
}

static void
sys_cntl(cntl_req_t* req, cntl_res_t* res)
{
  assert(req->mode == MODE_SYSTEM);

  if (req->op == OP_DOWN) {
    /* TODO: resource cleanup */
    res->len = sizeof(*res);
    res->status = 0;
  }
}

static int
process_cntl(cntl_job_t* job, int fd)
{
  int ret;
  cntl_req_t* req = &job->req;
  cntl_res_t* res = &job->res;

  if (req->mode == MODE_SYSTEM)
    sys_cntl(req, res);

  /* set send reply state */
  job->state = HDR_SEND;
  job->progress = 0;

  if ((ret = event_modify(fd, EPOLLOUT)))
    eprintf("failed to modify control job event out");

  return ret;
}

static void
recv_send_handler(int fd, int evtf, void* data)
{
  extern bool_t is_active;

  int len, ret;
  cntl_job_t* job = data;
  cntl_req_t* req = &job->req;
  cntl_res_t* res = &job->res;

  switch (job->state) {
    case HDR_RECV:
      len = sizeof(*req) - job->progress;
      if ((ret = read(fd, req + job->progress, len)) > 0) {
        job->progress += ret;
        if (job->progress == sizeof(*req)) {
          if ((ret = process_cntl(job, fd)))
            goto out;
        }
      } else if (errno != EAGAIN)
        goto out;
      break;
    case PDU_RECV:
      /* TODO: RECV PDU */
      break;
    case HDR_SEND:
      len = sizeof(*res) - job->progress;
      if ((ret = write(fd, res + job->progress, len)) > 0) {
        job->progress += ret;

        if (job->progress == sizeof(*res)) {
          if (job->progress == res->len)
            goto out;
          /* TODO: PDU_SEND */
        }
      } else if (errno != EAGAIN)
        goto out;
      break;
    case PDU_SEND:
      /* TODO: SEND PDU */
      break;
    default:
      eprintf("unknown state %d", job->state);
  }

  return;
out:
  if (req->mode == MODE_SYSTEM && req->op == OP_DOWN && res->status == 0)
    is_active = false;

  event_del(fd);
  close(fd);
  free_cntl_job(job);
}

static void
accept_handler(int accept_fd, int evtf, void* data)
{
  int fd = 0, flag;
  struct sockaddr addr;
  struct ucred cred;
  socklen_t len;
  cntl_job_t* job;

  /* ipc accept */
  len = sizeof(addr);
  if ((fd = accept(accept_fd, (struct sockaddr*)&addr, &len)) < 0) {
    eprintf_errno("failed to accept a new connection");
    return;
  }

  /* ipc permission */
  len = sizeof(cred);
  if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, (void*)&cred, &len) < 0) {
    eprintf_errno("failed to get sockopt");
    goto out;
  }

  if (cred.uid != getuid() || cred.gid != getgid()) {
    errno = EPERM;
    eprintf_errno("failed to accept a new connection");
    goto out;
  }

  /* non-blocking socket */
  if ((flag = fcntl(fd, F_GETFL)) < 0) {
    eprintf_errno("failed to get fd flags\n");
    goto out;
  }

  if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0) {
    eprintf_errno("failed to set fd O_NONBLOCK flags");
    goto out;
  }

  if (!(job = alloc_cntl_job()))
    goto out;

  if (event_add(fd, EPOLLIN, recv_send_handler, job) < 0) {
    eprintf("failed to add a socket to epoll: %d", fd);
    goto out;
  }

  return;
out:
  if (fd > 0)
    close(fd);
}

static int
mkdir_run(char* run_path)
{
  struct stat st;

  if (stat(run_path, &st))
    if (mkdir(run_path, 0755)) {
      eprintf_errno("failed to create directory '%s'", run_path);
      return -1;
    }

  return 0;
}

static int
check_lock_file(char* lock_path)
{
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  if ((lock_fd = open(lock_path, O_WRONLY | O_CREAT, mode)) < 0) {
    eprintf("failed to open lock file for control IPC");
    goto out;
  }

  if (lockf(lock_fd, F_TLOCK, 1) < 0) {
    if (errno == EACCES || errno == EAGAIN)
      eprintf("another instance is using %s", lock_path);
    else
      eprintf_errno("unable to get lock of control IPC: %s", lock_path);
    goto out;
  }

  return 0;
out:
  if (lock_fd > 0)
    close(lock_fd);

  return -1;
}

static int
open_ipc(char* ipc_path)
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

  if (event_add(fd, EPOLLIN, accept_handler, NULL))
    goto out;

  ipc_fd = fd;

  return 0;
out:
  close_cntl_ipc();
  return -1;
}

void
init_cntl_ipc(port_t cntl_port)
{
  extern const char* basename;

  char dir[128], path[256];

  snprintf(dir, SIZE_STR_BUF(dir), "/var/run/%s", basename);
  if (mkdir_run(dir))
    goto out;

  snprintf(path, SIZE_STR_BUF(path), "%s/socket.%u.lock", dir, cntl_port);
  if (check_lock_file(path))
    goto out;

  snprintf(path, SIZE_STR_BUF(path), "%s/socket.%d", dir, cntl_port);
  if (open_ipc(path))
    goto out;

  return;
out:
  if (ipc_fd > 0)
    close(ipc_fd);
  if (lock_fd > 0)
    close(lock_fd);

  exit(1);
}

void
close_cntl_ipc(void)
{
  // event_del(ipc_fd);
  close(ipc_fd);
  close(lock_fd);
}
