#include <asm-generic/errno.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "admin.h"
#include "log.h"
#include "util.h"

static const char* svc_name;

static int
connect_cntl_sock(char* host, port_t port)
{
  /* TODO: --host option */
  return -1;
}

static int
connect_cntl_ipc(port_t port)
{
  char path[256];
  struct sockaddr_un addr;
  int fd = 0;

  snprintf(path, SIZE_STR_BUF(path), "/var/run/%s/socket.%d", svc_name, port);
  if (strlen(path) > SIZE_STR_BUF(addr.sun_path)) {
    eprintf("control path too long: %s", path);
    errno = EINVAL;
    goto out;
  }

  if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    eprintf_errno("can't open a socket");
    goto out;
  }

  ZEROING(addr);
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, path);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    goto out;

  return fd;
out:
  if (fd > 0)
    close(fd);

  return -1;
}

static int inline connect_svc(admin_job_t* job)
{
  int fd = -1;

  /* TODO: connect_sock(req->host, req->port); */
  fd = connect_cntl_ipc(job->port);

  return fd;
}

static int
recv_response(int fd, admin_job_t* job)
{
  int ret, remained;
  int res; /* XXX: tmp type */

  remained = sizeof(res);
  ret = 0;

retry:
  if ((ret = recv(fd, &res, remained, MSG_WAITALL)) < 0) {
    if (errno == EAGAIN)
      goto retry;
    else if (errno == EINTR)
      eprintf("interrupted by a signal");

    return errno;
  } else if (ret == 0) {
    eprintf("%s closed the socket\n", svc_name);
    return -1;
  } else if (ret < remained) {
    remained -= ret;
    goto retry;
  }

  if (job->req.mode == MODE_SYSTEM && job->req.op == OP_DOWN) {
    /* wait_svc_down */
    while (true) {
      int _fd = connect_svc(job);
      if (errno == ECONNREFUSED)
        break;

      close(fd);
      usleep(100 * 1000); /* 0.1sec */
    }
  }

  /* TODO:response handle */

  return 0;
}

int
send_request(admin_job_t* job)
{
  int fd = 0, len, ret = -1;
  void* buf;

  svc_name = SVC_T2STR(job->req.svc);
  assert(strlen(svc_name) > 0);

  if ((fd = connect_svc(job)) < 0) {
    eprintf_errno("failed to connect to %s", svc_name);
    goto out;
  }

  /* XXX: tmp, no pdu */
  job->req.len = 0;

  len = sizeof(job->req) + job->req.len;
  ret = write(fd, &job->req, len);
  if (ret < 0 || ret != len) {
    eprintf_errno("failed to send request to %s", svc_name);
    ret = errno;
    goto out;
  }

  ret = recv_response(fd, job);

out:
  if (fd > 0)
    close(fd);

  return ret;
}
