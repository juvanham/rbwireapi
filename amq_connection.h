#ifndef AMQ_CONNECTION_HEADER
#define AMQ_CONNECTION_HEADER

#include<ruby.h>

void init_connection();
VALUE con_client_connection_destroy(VALUE self);

#endif
