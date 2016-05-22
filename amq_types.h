#ifndef AMQ_TYPES_HEADER
#define AMQ_TYPES_HEADER

#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"
#include "amq_client_agent.h"
#include "amq_exchange.h"

extern VALUE amqp_content;

extern VALUE rb_wireapi;

extern VALUE amqp_connection;

extern VALUE amqp_session;

extern VALUE amqp_auth;

extern VALUE amqp_field_list;

extern VALUE rb_amq_error;

extern VALUE rb_amq_server_error;

extern VALUE amqp_int64;


struct _amq_session_cls {
  amq_client_session_t  *session;
};

typedef struct _amq_session_cls amq_session_cls;

/* */

struct _amq_content_cls {
  amq_content_basic_t  *content;
};

typedef struct _amq_content_cls amq_content_cls;
/* */

struct _amq_connection_cls {
  amq_client_connection_t  *connection;
};

typedef struct _amq_connection_cls amq_connection_cls;



struct _amq_auth_cls {
  icl_longstr_t  *auth;
};

typedef struct _amq_auth_cls amq_auth_cls;

struct _amq_int64_cls {
  int64_t value;
};

typedef struct _amq_int64_cls amq_int64_cls;


struct _amq_field_list_cls {
  asl_field_list_t *field_list;
};

typedef struct _amq_field_list_cls amq_field_list_cls;



struct _amq_wire_api_cls {
  int instance_id;
};

typedef struct _amq_wire_api_cls amq_wire_api_cls;


/*  session allocator, destructor , accessor */
amq_session_cls* get_amq_session_cls(VALUE rb_obj,int raise);
void free_amq_session_cls( amq_session_cls *obj);
VALUE create_amq_session_cls(amq_client_session_t  *session);


/*  content allocator, destructor , accessor */
amq_content_cls* get_amq_content_cls(VALUE rb_obj,int raise);
void free_amq_content_cls( amq_content_cls *obj);
VALUE create_amq_content_cls(amq_content_basic_t  *content);
 

/*  connection allocator, destructor , accessor */
amq_connection_cls* get_amq_connection_cls(VALUE rb_obj,int raise);
void free_amq_connection_cls( amq_connection_cls *obj);
VALUE create_amq_connection_cls(amq_client_connection_t  *connection);

 

/*  connection allocator, destructor , accessor */
amq_auth_cls* get_amq_auth_cls(VALUE rb_obj,VALUE raise);
void free_amq_auth_cls( amq_auth_cls *obj) ;
VALUE create_amq_auth_cls(icl_longstr_t *auth);


/*  int64_t allocator, destructor , accessor */
amq_int64_cls* get_amq_int64_cls(VALUE rb_obj) ;
void free_amq_int64_cls( amq_int64_cls *obj) ;
VALUE create_amq_int64_cls(int64_t int64);


/*  field_list allocator, destructor , accessor */
amq_field_list_cls* get_amq_field_list_cls(VALUE rb_obj,VALUE raise);
void free_amq_field_list_cls( amq_field_list_cls *obj) ;
VALUE create_amq_field_list_cls(asl_field_list_t *field_list);


/*  wire_api allocator, destructor , accessor */
amq_wire_api_cls* get_amq_wire_api_cls(VALUE rb_obj,VALUE raise);
void free_amq_wire_api_cls( amq_wire_api_cls *obj) ;
VALUE create_amq_wire_api_cls();




int VALUE2bool (VALUE v,const char *msg);


#endif
