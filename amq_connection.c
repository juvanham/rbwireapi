
/**********************************************
 * Document-class:  WireAPI::AMQ_Connection
 *
 *
 **********************************************/


#include <ruby.h>
#include "amq_types.h"
#include "amq_connection.h"
#include <amq_client_connection.h>

VALUE amqp_connection;

/*
 * Document-method: alive
 * call-seq:
 *   AMQ_Connection#alive -> boolean
 */
static VALUE con_alive(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  if (amq_client_connection_get_alive(connection_obj->connection)) 
	return Qtrue;
  else
	return Qfalse;
}

/* Document-method: client_session_new
 *
 * AMQ_Connection#client_session_new -> AMQP_Session
 */  
static VALUE con_client_session_new(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  amq_client_session_t *session = NULL;  
  VALUE rb_session;    
  if (connection_obj) {
    session = amq_client_session_new (connection_obj->connection);
    rb_session = create_amq_session_cls(session);
  } else {
    rb_raise(rb_eRuntimeError,"Could not connect to server");
  }
  
  if (rb_block_given_p()) {
	rb_yield(rb_session);
	ses_client_session_destroy(rb_session);
	return Qtrue;
  } else {
	return rb_session ;
  }
}



/*
 * AMQ_Connection#destroy -> nil
 *
 */
VALUE con_client_connection_destroy(VALUE self)
{ 
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,0);
  if (connection_obj && connection_obj->connection) {
    amq_client_connection_destroy(&(connection_obj->connection));
    connection_obj->connection=0;
  }
  return Qnil;
}


/*
 * AMQ_Connection#error_text -> string
 */
static VALUE con_error_text(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  const char *error_text=amq_client_connection_get_error_text(connection_obj->connection);
  return rb_str_new2(error_text);
}


/*
 * AMQ_Connection#reply_code -> Fixnum
 */
static VALUE con_reply_code(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  int reply_code=amq_client_connection_get_reply_code(connection_obj->connection);
  return INT2FIX(reply_code);
}

/*
 * AMQ_Connection#reply_text -> string
 */
static VALUE con_reply_text(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  const char *reply_text=amq_client_connection_get_reply_text(connection_obj->connection);
  return rb_str_new2(reply_text);
}

/*
 * AMQ_Connection#server_copyright -> string
 */
static VALUE con_server_copyright(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  return rb_str_new2(connection_obj->connection->server_copyright);
}

/*
 * AMQ_Connection#server_information -> string
 */
static VALUE con_server_information(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  return rb_str_new2(connection_obj->connection->server_information);
}

/*
 * AMQ_Connection#server_platform -> string
 */
static VALUE con_server_platform(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  return rb_str_new2(connection_obj->connection->server_platform);
}

/*
 * AMQ_Connection#server_product -> string
 */
static VALUE con_server_product(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  return rb_str_new2(connection_obj->connection->server_product);
}

/*
 * AMQ_Connection#server_version -> string
 */
static VALUE con_server_version(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  return rb_str_new2(connection_obj->connection->server_version);
}


/*
 * AMQ_Connection#silent -> boolean
 */
static VALUE con_get_silent(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  if ( amq_client_connection_get_silent(connection_obj->connection))
	return Qtrue;
  else
	return Qfalse;
}


/*
 * AMQ_Connection#silent=(boolean) -> self
 */
static VALUE con_set_silent(VALUE self,VALUE nval)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  int result=0;
  if (nval==Qtrue) {
	result=amq_client_connection_set_silent(connection_obj->connection,1);
  } else if (nval==Qfalse) {
	result=amq_client_connection_set_silent(connection_obj->connection,0);
  } else {
	rb_raise(rb_amq_error,"AMQ_Connection#silent=(boolean) argument not boolean");
  }
  if (result) {
	rb_raise(rb_amq_error,"amq_client_connection_set_silent(..) failed");
  }
  return self;
}


/*
 * AMQ_Connection#version_major -> Fixnum
 */
static VALUE con_version_major(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  int version_major=amq_client_connection_get_version_major(connection_obj->connection);
  return INT2FIX(version_major);
}

/*
 * AMQ_Connection#version_minor -> Fixnum
 */
static VALUE con_version_minor(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  int version_minor=amq_client_connection_get_version_minor(connection_obj->connection);
  return INT2FIX(version_minor);
}

/*
 * AMQ_Connection#version -> String
 */
static VALUE con_version(VALUE self)
{
  amq_connection_cls *const connection_obj=get_amq_connection_cls(self,1);
  const int len=80;
  char buffer[len+1];
  int version_major=amq_client_connection_get_version_major(connection_obj->connection);
  int version_minor=amq_client_connection_get_version_minor(connection_obj->connection);
  snprintf(buffer,len,"%i.%i",version_major,version_minor);
  return rb_str_new2(buffer);
}




void init_connection() {
  amqp_connection = rb_define_class_under(rb_wireapi,"AMQ_Connection",rb_cObject); // in amq_client.c
  rb_define_method(amqp_connection,"alive",con_alive,0);
  rb_define_method(amqp_connection,"client_session_new", con_client_session_new,0);  
  rb_define_method(amqp_connection,"destroy",con_client_connection_destroy,0);
  rb_define_method(amqp_connection,"error_text",con_error_text,0);
  rb_define_method(amqp_connection,"reply_code",con_reply_code,0);
  rb_define_method(amqp_connection,"reply_text",con_reply_text,0);
  rb_define_method(amqp_connection,"server_copyright",con_server_copyright,0);
  rb_define_method(amqp_connection,"server_information",con_server_information,0);
  rb_define_method(amqp_connection,"server_platform",con_server_platform,0);
  rb_define_method(amqp_connection,"server_product",con_server_product,0);
  rb_define_method(amqp_connection,"server_version",con_server_version,0);
  rb_define_method(amqp_connection,"silent",con_get_silent,0);
  rb_define_method(amqp_connection,"silent=",con_set_silent,1);
  rb_define_method(amqp_connection,"version_major",con_version_major,0);
  rb_define_method(amqp_connection,"version_minor",con_version_minor,0);
  rb_define_method(amqp_connection,"version",con_version,0);
}
