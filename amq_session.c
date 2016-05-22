/**********************************************
 * Document-class: AMQ_Session
 *
 *
 **********************************************/



#include <ruby.h>
#include "amq_client_session.h"
#include "amq_types.h"
#include "amq_field_list.h"

VALUE amqp_int64;
VALUE amqp_session;



static VALUE amq_server_error_reply_code(VALUE self)  {
  return rb_iv_get(self,"reply_code");
}


/* when reply code is not 0 throw a server error */
int check_reply_code(VALUE self) {
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);  
  int reply_code=session_obj->session->reply_code;
  if (reply_code) {
	VALUE reply_text=rb_str_new2(session_obj->session->reply_text);
	VALUE _reply_code= INT2NUM(reply_code);
	VALUE argv[1];
	argv[0]=reply_text;
	VALUE exception=rb_class_new_instance(1,argv, rb_amq_server_error);
	rb_iv_set(exception, "reply_code", _reply_code);
	rb_exc_raise(exception);
	return -1;
  }
  return 0;
}

/*
 * AMQ_Session#alive() -> boolean
 *
 * session is paused or active
 */
static VALUE ses_active(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);  
  if (amq_client_session_get_active(session_obj->session))
	return Qtrue;
  else
	return Qfalse;
}

/*
 * AMQ_Session#alive() -> boolean
 *
 * FALSE when connection has had an error
 */
static VALUE ses_alive(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);  
  if  (amq_client_session_get_alive(session_obj->session)) 
	return Qtrue;
  else
	return Qfalse;

}


/*
 * AMQ_Session#error_text -> string
 */
static VALUE ses_error_text(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(session_obj->session->error_text);
}

/*
 * AMQ_Session#destroy() -> nil
 *
 * After calling this method the session object will become unusable since the
 * internal structures will be released
 */
VALUE ses_client_session_destroy(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,0);
  if (session_obj && session_obj->session) {  
    amq_client_session_destroy (&(session_obj->session));
    session_obj->session=0;	
  }
  return Qnil;
}

/*
 * AMQ_Session#exchange -> string
 */
static VALUE ses_exchange(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(amq_client_session_get_exchange(session_obj->session));
}


/*
 * AMQ_Session#message_count -> number
 */
static VALUE ses_message_count(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return INT2FIX(amq_client_session_get_message_count(session_obj->session));
}


/*
 * AMQ_Session#consumer_count -> number
 */
static VALUE ses_consumer_count(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return INT2FIX(amq_client_session_get_consumer_count(session_obj->session));
}

/*
 * AMQ_Session#queue -> string
 */
static VALUE ses_queue(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(session_obj->session->queue);
}

/*
 * AMQ_Session#reply_code -> number
 */
static VALUE ses_reply_code(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return INT2FIX(session_obj->session->reply_code);
}

/*
 * AMQ_Session#reply_text -> string
 */
static VALUE ses_reply_text(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(session_obj->session->reply_text);
}

/*
 * AMQ_Session#wait(timeout) -> number
 *
 * * timeout FixNum
 */
static VALUE ses_client_session_wait(int argc,VALUE *argv,VALUE self)
{
  VALUE timeout;
  rb_scan_args(argc,argv,"01",&timeout);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  if (timeout==Qnil) {
	timeout=INT2NUM(0);
  }
  if( FIXNUM_P(timeout))
    {
      int result = amq_client_session_wait (session_obj->session,NUM2INT(timeout));
      if (result) {
		rb_raise(rb_amq_error,session_obj->session->error_text);
	  }
    } 
  else 
    {
      rb_raise(rb_eTypeError,"Timeout was not an Fixnum");
    }
  return self;
}

/*
 * AMQ_Session#exchange_declare(exchangeName,type,passive,durable,autodelete,internal) -> self
 *
 * * exchangeName : String
 * * type : String from "fanout", "direct", "topic", and "header"
 * * passive : boolean  true will create
 * * durable : boolean 
 * * autodelete: boolean
 * * internal : boolean
 */

static VALUE ses_client_session_exchange_declare(int argc,VALUE *argv,VALUE self) {
  VALUE exchange;
  VALUE type;
  VALUE passive;
  VALUE durable;
  VALUE autodelete;
  VALUE internal;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"61",&exchange,&type,&passive,&durable,&autodelete,&internal,&argument_table_opt);
  
  Check_Type(exchange,T_STRING);
  Check_Type(type,T_STRING);
  char *_exchange = StringValuePtr(exchange);
  char *_type = StringValuePtr(type);
  int _passive=VALUE2bool(passive,"arg: passive not boolean");
  int _durable=VALUE2bool(durable,"arg: durable not boolean");
  int _autodelete=VALUE2bool(autodelete,"arg: autodelete not boolean");
  int _internal=VALUE2bool(internal,"arg: internal not boolean");
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_exchange_declare(session_obj->session,0,
												 _exchange,
												 _type,
												 _passive,
												 _durable,
												 _autodelete,
												 _internal,
												 _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}



static VALUE ses_client_session_exchange_declare_nowait(int argc,VALUE *argv,VALUE self) {
  VALUE exchange;
  VALUE type;
  VALUE passive;
  VALUE durable;
  VALUE autodelete;
  VALUE internal;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"61",&exchange,&type,&passive,&durable,&autodelete,&internal,&argument_table_opt);
  
  Check_Type(exchange,T_STRING);

  char *_exchange = StringValuePtr(exchange);
  char *_type = StringValuePtr(type);
  int _passive=VALUE2bool(passive,"arg: passive not boolean");
  int _durable=VALUE2bool(durable,"arg: durable not boolean");
  int _autodelete=VALUE2bool(autodelete,"arg: autodelete not boolean");
  int _internal=VALUE2bool(internal,"arg: internal not boolean");
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_exchange_declare_nowait(session_obj->session,0,
												 _exchange,
												 _type,
												 _passive,
												 _durable,
												 _autodelete,
												 _internal,
												 _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#exchange_delete(exchangeName) -> self
 *
 * * exchangeName : String
 */
static VALUE ses_client_session_exchange_delete(VALUE self, 
												VALUE exchange)
{
  Check_Type(exchange,T_STRING);
  char *_exchange = StringValuePtr(exchange);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_exchange_delete(session_obj->session,0,_exchange,0);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}

/*
 * AMQ_Session#exchange_delete_nowait(exchangeName) -> self
 *
 * * exchangeName : String
 */
static VALUE ses_client_session_exchange_delete_nowait(VALUE self, 
													   VALUE exchange)
{
  Check_Type(exchange,T_STRING);
  char *_exchange = StringValuePtr(exchange);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_exchange_delete_nowait(session_obj->session,0,_exchange,0);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}




/*
 * AMQ_Session#exchange_declare(queueName,passive,durable,exclusive,autodelete) -> self
 * 
 * * queueName : String
 * * passive : boolean  true will create
 * * durable : boolean 
 * * exclusive : boolean 
 * * autodelete: boolean
 */
static VALUE ses_client_session_queue_declare(int argc,VALUE *argv,VALUE self)
{
  VALUE queuename; 
  VALUE passive;
  VALUE durable;
  VALUE exclusive;
  VALUE autodelete;
  VALUE argument_table_opt;

  rb_scan_args(argc,argv,"51",&queuename, &passive, &durable, &exclusive,&autodelete,&argument_table_opt);

  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);

  int _passive=VALUE2bool(passive,"arg: passive not boolean");
  int _durable=VALUE2bool(durable,"arg: durable not boolean");
  int _exclusive=VALUE2bool(exclusive,"arg: exclusive not boolean");
  int _autodelete=VALUE2bool(autodelete,"arg: autodelete not boolean");
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  int result=amq_client_session_queue_declare(session_obj->session,0,
											  _queuename,
											  _passive,
											  _durable,
											  _exclusive,
											  _autodelete,
											  _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


static VALUE ses_client_session_queue_declare_nowait(int argc,VALUE *argv,VALUE self)
{
  VALUE queuename; 
  VALUE passive;
  VALUE durable;
  VALUE exclusive;
  VALUE autodelete;
  VALUE argument_table_opt;

  rb_scan_args(argc,argv,"51",&queuename, &passive, &durable, &exclusive,&autodelete,&argument_table_opt);

  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);

  int _passive=VALUE2bool(passive,"arg: passive not boolean");
  int _durable=VALUE2bool(durable,"arg: durable not boolean");
  int _exclusive=VALUE2bool(exclusive,"arg: exclusive not boolean");
  int _autodelete=VALUE2bool(autodelete,"arg: autodelete not boolean");
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  int result=amq_client_session_queue_declare_nowait(session_obj->session,0,
													 _queuename,_passive,
													 _durable,
													 _exclusive,
													 _autodelete,
													 _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#queue_delete(queuename) -> self
 * * queuename : string
 */
static VALUE ses_client_session_queue_delete(VALUE self, 
											 VALUE queuename)
{
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);


  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_delete(session_obj->session,0,_queuename,0,0);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}

/*
 * AMQ_Session#queue_delete_nowait(queuename) -> self
 * * queuename : string
 */
static VALUE ses_client_session_queue_delete_nowait(VALUE self, 
													VALUE queuename)
{
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_delete_nowait(session_obj->session,0,_queuename,0,0);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


       


static VALUE ses_client_session_queue_bind(int argc, VALUE *argv,VALUE self) {
  VALUE queuename;
  VALUE exchange;
  VALUE routing_key;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"31",&queuename,&exchange,&routing_key,&argument_table_opt);

  Check_Type(exchange,T_STRING);
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  char *_exchange =StringValuePtr(exchange);
  char *_routing_key = StringValuePtr(routing_key);
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);

  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_bind(session_obj->session,
										   0,
										   _queuename,
										   _exchange,
										   _routing_key,
										   _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }

  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}

       


static VALUE ses_client_session_queue_bind_nowait(int argc, VALUE *argv,VALUE self) {
  VALUE queuename;
  VALUE exchange;
  VALUE routing_key;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"31",&queuename,&exchange,&routing_key,&argument_table_opt);
  Check_Type(exchange,T_STRING);
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  char *_exchange =StringValuePtr(exchange);
  char *_routing_key = StringValuePtr(routing_key);
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);

  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_bind_nowait(session_obj->session,
										   0,
										   _queuename,
										   _exchange,
										   _routing_key,
										   _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }

  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}

       

static VALUE ses_client_session_queue_unbind(int argc,VALUE *argv,VALUE self) {
  VALUE queuename;
  VALUE exchange;
  VALUE routing_key;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"31",&queuename,&exchange, &routing_key,&argument_table_opt);

  char *_queuename = StringValuePtr(queuename);
  char *_exchange = StringValuePtr(exchange);
  char *_routing_key = StringValuePtr(routing_key);
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_unbind(session_obj->session,0,
											 _queuename,
											 _exchange,
											 _routing_key,
											 _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}



static VALUE ses_client_session_queue_unbind_nowait(int argc,VALUE *argv,VALUE self) {
  VALUE queuename;
  VALUE exchange;
  VALUE routing_key;
  VALUE argument_table_opt;
  rb_scan_args(argc,argv,"31",&queuename,&exchange, &routing_key,&argument_table_opt);

  char *_queuename = StringValuePtr(queuename);
  char *_exchange = StringValuePtr(exchange);
  char *_routing_key = StringValuePtr(routing_key);
  icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_unbind_nowait(session_obj->session,0,
											 _queuename,
											 _exchange,
											 _routing_key,
											 _argument_table);
  if ( _argument_table) {
	icl_longstr_destroy(& _argument_table);
	_argument_table=0;
  }
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#client_session_queue_purge(queuename) -> self
 * * queuename : string
 */
static VALUE ses_client_session_queue_purge(VALUE self,
											VALUE queuename)
{
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_purge(session_obj->session,0,_queuename);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#client_session_queue_purge_nowait(queuename) -> self
 * * queuename : string
 */
static VALUE ses_client_session_queue_purge_nowait(VALUE self,
												   VALUE queuename)
{
  char *_queuename = 0;
  if (queuename!=Qnil)
	_queuename=StringValuePtr(queuename);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int result=amq_client_session_queue_purge(session_obj->session,0,_queuename);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }	  
  check_reply_code(self);
  return self;
}



static VALUE ses_client_session_basic_consume(int argc,VALUE *argv,VALUE self) {
	VALUE queuename;
	VALUE consumer_tag;
	VALUE no_local;
	VALUE no_ack;
	VALUE exclusive;
	VALUE argument_table_opt;
	rb_scan_args(argc,argv,"51",&queuename,&consumer_tag,&no_local,&no_ack,&exclusive,&argument_table_opt);

	char *_queuename = StringValuePtr(queuename);
	char *_consumer_tag = 0;
	if (TYPE(consumer_tag)==T_STRING)
	  _consumer_tag=StringValuePtr(consumer_tag);
	int _no_local=VALUE2bool(no_local,"arg: no_local not boolean");
	int _no_ack=VALUE2bool(no_ack,"arg: no_ack not boolean");
	int _exclusive=VALUE2bool(no_local,"arg: exclusive not boolean");
	icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);

	amq_session_cls *const session_obj=get_amq_session_cls(self,1);
	int result = amq_client_session_basic_consume(session_obj->session,0,
												  _queuename,
												  _consumer_tag,
												  _no_local,
												  _no_ack,
												  _exclusive,
												  _argument_table);
	if ( _argument_table) {
	  icl_longstr_destroy(& _argument_table);
	  _argument_table=0;
	}
	if (result) {
	  rb_raise(rb_amq_error,session_obj->session->error_text);
	}	  
  check_reply_code(self);
  return self;
}



static VALUE ses_client_session_basic_consume_nowait(int argc,VALUE *argv,VALUE self) {
	VALUE queuename;
	VALUE consumer_tag;
	VALUE no_local;
	VALUE no_ack;
	VALUE exclusive;
	VALUE argument_table_opt;
	rb_scan_args(argc,argv,"51",&queuename,&consumer_tag,&no_local,&no_ack,&exclusive,&argument_table_opt);

	char *_queuename = StringValuePtr(queuename);
	char *_consumer_tag = 0;
	if (TYPE(consumer_tag)==T_STRING)
	  _consumer_tag=StringValuePtr(consumer_tag);
	int _no_local=VALUE2bool(no_local,"arg: no_local not boolean");
	int _no_ack=VALUE2bool(no_ack,"arg: no_ack not boolean");
	int _exclusive=VALUE2bool(no_local,"arg: exclusive not boolean");
	icl_longstr_t *_argument_table=get_optional_field_list(argument_table_opt);

	amq_session_cls *const session_obj=get_amq_session_cls(self,1);
	int result = amq_client_session_basic_consume_nowait(session_obj->session,0,
												  _queuename,
												  _consumer_tag,
												  _no_local,
												  _no_ack,
												  _exclusive,
												  _argument_table);
	if ( _argument_table) {
	  icl_longstr_destroy(& _argument_table);
	  _argument_table=0;
	}
	if (result) {
	  rb_raise(rb_amq_error,session_obj->session->error_text);
	}	  
  check_reply_code(self);
  return self;
}





/* AMQ_Session#basic_public(content,exchange,routing_key,mandatory)
 *
 * * content : AMQP_Content
 * * exchange : String
 * * routing_key : string
 * * mandatory  : boolean
 * * immediate  : boolean
 * 
 */
static VALUE ses_client_session_basic_publish(VALUE self,
											  VALUE content,
											  VALUE exchange,
											  VALUE routing_key,
											  VALUE mandatory,
											  VALUE immediate)
{
  Check_Type(exchange,T_STRING);
  Check_Type(routing_key,T_STRING);
  char *_exchange = StringValuePtr(exchange);
  char *_routing_key = StringValuePtr(routing_key);
  int _mandatory=VALUE2bool(mandatory,"arg: mandatory not boolean");
  int _immediate=VALUE2bool(immediate,"arg: immediate not a boolean");
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  amq_content_cls *const content_obj=get_amq_content_cls(content,1);

  int result=amq_client_session_basic_publish( 
											  session_obj->session, 
											  content_obj->content, 0, 
											  _exchange,_routing_key,
											  _mandatory, _immediate);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }
  check_reply_code(self);
  return self;
}



/* AMQ_Session#basic_get(queue_name,no_ack)-> self
 *
 * * queue_name : string
 * * no_ack     : boolean
 */
static VALUE ses_client_session_basic_get(VALUE self,VALUE queue_name,VALUE no_ack)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  Check_Type(queue_name,T_STRING);
  char *_queuename=StringValuePtr(queue_name);
  int _no_ack=VALUE2bool(no_ack,"no ack arguments needs to be a boolean");
	
  int res=amq_client_session_basic_get(session_obj->session,0,_queuename,_no_ack);
  if (res) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }
  check_reply_code(self);
  return self;	
}





/* AMQ_Session#basic_arrived -> AMQP_Content or nil
 *
 */
static VALUE ses_client_session_basic_arrived(VALUE self)
{
  VALUE rb_content;
  amq_content_basic_t *content = NULL;
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  content = amq_client_session_basic_arrived(session_obj->session);
  check_reply_code(self);
  if (content) {
	rb_content = create_amq_content_cls(content);
	return rb_content;
  } else {
	return Qnil;
  }   
}



/* AMQ_Session#basic_cancel(consumer_tag)
 *
 * consumer_tag : string  
 * 
 */
static VALUE ses_client_session_basic_cancel(VALUE self,
											 VALUE consumer_tag)
{
  int result;
  char *_consumer_tag = StringValuePtr(consumer_tag);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  result = amq_client_session_basic_cancel(session_obj->session,_consumer_tag);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }
  check_reply_code(self);
  return self;
}


/* AMQ_Session#basic_cancel_nowait(consumer_tag)
 *
 * * consumer_tag : string 
 * 
 */
static VALUE ses_client_session_basic_cancel_nowait(VALUE self,
													VALUE consumer_tag)
{
  int result;
  char *_consumer_tag = StringValuePtr(consumer_tag);
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  result = amq_client_session_basic_cancel_nowait(session_obj->session,_consumer_tag);
  if (result) {
	rb_raise(rb_amq_error,session_obj->session->error_text);
  }
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#consumer_tag -> string
 */
static VALUE ses_consumer_tag(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(amq_client_session_get_consumer_tag(session_obj->session));
}


/*
 * AMQ_Session#routing_key -> string
 */
static VALUE ses_routing_key(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  return rb_str_new2(amq_client_session_get_routing_key(session_obj->session));
}




/*
 * AMQ_Session#delivery_tag -> AMQP_int64
 *
 */

static VALUE ses_delivery_tag(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int64_t tag=amq_client_session_get_delivery_tag(session_obj->session);
  return create_amq_int64_cls(tag);
}

/*
 * AMQ_Session#basic_ack (delivery_tag,multiple)
 *
 * * delivery_tag : AMQP_int64
 * * multiple : boolean
 *
 */
static VALUE ses_basic_ack(VALUE self,VALUE delivery,VALUE multiple)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int64_t tag=get_amq_int64_cls(delivery)->value;
  int _multiple=VALUE2bool(multiple,"multiple is a boolean");
  amq_client_session_basic_ack(session_obj->session,tag,_multiple);
  check_reply_code(self);
  return self;
}



/*
 * AMQ_Session#basic_reject (delivery_tag,requeue)
 *
 * * delivery_tag : AMQP_int64
 * * requeue : boolean
 *
 */
static VALUE ses_basic_reject(VALUE self,VALUE delivery,VALUE requeue)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  int64_t tag=get_amq_int64_cls(delivery)->value;
  int _requeue=VALUE2bool(requeue,"requeue is a boolean");
  amq_client_session_basic_reject(session_obj->session,tag,_requeue);
  check_reply_code(self);
  return self;
}


/*
 * AMQ_Session#redelivered -> bool
 *
 *
 */
static VALUE ses_redelivered(VALUE self)
{
  amq_session_cls *const session_obj=get_amq_session_cls(self,1);
  if (amq_client_session_get_redelivered(session_obj->session)) 
	return Qtrue;
  else	
	return Qfalse;
}


void init_session() {
  amqp_int64= rb_define_class_under(rb_wireapi,"AMQ_int64",rb_cObject);

  rb_define_method(rb_amq_server_error,"reply_code"    , amq_server_error_reply_code,0);

  // Session
  amqp_session = rb_define_class_under(rb_wireapi,"AMQ_Session",rb_cObject);
 


  rb_define_method(amqp_session,"active"       , ses_active,0);
  rb_define_method(amqp_session,"alive"        , ses_alive ,0); 
  rb_define_method(amqp_session,"basic_ack"    , ses_basic_ack,2);
  rb_define_method(amqp_session,"basic_arrived", ses_client_session_basic_arrived,0);
  rb_define_method(amqp_session,"basic_cancel" , ses_client_session_basic_cancel,1);  
  rb_define_method(amqp_session,"basic_cancel_nowait"    , ses_client_session_basic_cancel_nowait,1);  
  rb_define_method(amqp_session,"basic_consume", ses_client_session_basic_consume,-1); 
  rb_define_method(amqp_session,"basic_consume_nowait"   , ses_client_session_basic_consume_nowait,-1); 
  rb_define_method(amqp_session,"basic_get"    , ses_client_session_basic_get,2);
  rb_define_method(amqp_session,"basic_publish", ses_client_session_basic_publish,5); 
  rb_define_method(amqp_session,"basic_reject" , ses_basic_reject,2);
  rb_define_method(amqp_session,"consumer_count",ses_consumer_count,0);
  rb_define_method(amqp_session,"consumer_tag" , ses_consumer_tag,0);
  rb_define_method(amqp_session,"delivery_tag" , ses_delivery_tag,0);
  rb_define_method(amqp_session,"destroy"      , ses_client_session_destroy,0);
  rb_define_method(amqp_session,"error_text"   , ses_error_text,0);
  rb_define_method(amqp_session,"exchange"     , ses_exchange,0);
  rb_define_method(amqp_session,"exchange_declare", ses_client_session_exchange_declare,-1); 
  rb_define_method(amqp_session,"exchange_declare_nowait", ses_client_session_exchange_declare_nowait,-1); 
  rb_define_method(amqp_session,"exchange_delete"        , ses_client_session_exchange_delete,1); 
  rb_define_method(amqp_session,"exchange_delete_nowait" , ses_client_session_exchange_delete_nowait,1); 
  rb_define_method(amqp_session,"message_count", ses_message_count,0);
  rb_define_method(amqp_session,"queue"        , ses_queue,0);
  rb_define_method(amqp_session,"queue_bind"   , ses_client_session_queue_bind,-1); 
  rb_define_method(amqp_session,"queue_bind_nowait"      , ses_client_session_queue_bind_nowait,-1); 
  rb_define_method(amqp_session,"queue_declare", ses_client_session_queue_declare,-1); 
  rb_define_method(amqp_session,"queue_declare_nowait"   , ses_client_session_queue_declare_nowait,-1); 
  rb_define_method(amqp_session,"queue_delete" , ses_client_session_queue_delete,1); 
  rb_define_method(amqp_session,"queue_delete_nowait"    , ses_client_session_queue_delete_nowait,1); 
  rb_define_method(amqp_session,"queue_purge"  , ses_client_session_queue_purge,1); 
  rb_define_method(amqp_session,"queue_purge_nowait"     , ses_client_session_queue_purge_nowait,1); 
  rb_define_method(amqp_session,"queue_unbind" , ses_client_session_queue_unbind,-1); 
  rb_define_method(amqp_session,"queue_unbind_nowait"    , ses_client_session_queue_unbind_nowait,-1); 
  rb_define_method(amqp_session,"redelivered"  , ses_redelivered,0);
  rb_define_method(amqp_session,"reply_code"   , ses_reply_code,0);
  rb_define_method(amqp_session,"reply_text"   , ses_reply_text,0);
  rb_define_method(amqp_session,"routing_key"  , ses_routing_key,0);
  rb_define_method(amqp_session,"wait"    , ses_client_session_wait,-1); 

}
