#ifndef __ADMIN_H__
#define __ADMIN_H__

#include "svc.h"
#include "types.h"

typedef struct
{
  port_t port;
  svc_type_t svc;

  cntl_req_t req;
} admin_job_t;

/* transport.c */
extern int
send_request(admin_job_t*);

#endif // !__ADMIN_H__
