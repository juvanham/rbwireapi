#ifndef AMQ_SESSION_HEADER
#define AMQ_SESSION_HEADER

#include <ruby.h>

void init_session();
VALUE ses_client_session_destroy(VALUE self);

#endif
