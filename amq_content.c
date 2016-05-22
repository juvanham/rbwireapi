

/**********************************************
 * WireAPI::AMQ_ContentBasic
 *
 *
 **********************************************/

#include <ruby.h>
#include <asl.h>
#include "amq_types.h"
#include <amq_content_basic.h>
#include "amq_content.h"
#include "amq_field_list.h"

VALUE amqp_content;


static VALUE cont_content_basic_new(VALUE self)
{
  VALUE rb_content=create_amq_content_cls(amq_content_basic_new ());
  return rb_content;
}



/*
 * AMQ_ContentBasic#destroy() -> self
 */
static VALUE cont_content_basic_destroy(VALUE self)
{
  amq_content_cls* content_obj=get_amq_content_cls(self,0);
  if (content_obj && content_obj->content) {
    amq_content_basic_unlink(&(content_obj->content));
    content_obj->content=0;
  }
  return Qnil;
}



/*
 * AMQ_ContentBasic#app_id -> string
 *
 */
static VALUE cont_content_basic_get_app_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const app_id=amq_content_basic_get_app_id(content_obj->content);
  return rb_str_new2(app_id);
}

/*
 * AMQ_ContentBasic#app_id=(string) -> self
 *
 */
static VALUE cont_content_basic_set_app_id(VALUE self,VALUE appid)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(appid,T_STRING);
  char *_app_id = StringValuePtr(appid);

  amq_content_basic_set_app_id(content_obj->content,_app_id);
  return self;
}

/*
 * AMQ_ContentBasic#body=(string) -> self
 */
static VALUE cont_content_basic_set_body(VALUE self,VALUE value)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(value,T_STRING);
  char *_value = strdup(StringValuePtr(value));
  amq_content_basic_set_body (content_obj->content, 
							  _value, 
							  RSTRING_LEN(value), 
							  free);
  return self;
}

/*
 *  AMQ_ContentBasic#body(size) -> String
 *  the maximum size that will be accepted
 *  0 or nil means no limit
 */
static VALUE cont_content_basic_get_body(int argc, VALUE *argv, VALUE self) { 
  amq_content_cls *const content_obj=get_amq_content_cls(self,1);
  VALUE size;
  rb_scan_args(argc,argv,"01",&size);
  int _size=0;
  if (TYPE(size)==T_FIXNUM)
	_size=NUM2INT(size);
  if (_size==0)
	_size=2+amq_content_basic_get_body_size(content_obj->content);
  char *const _value= malloc(_size+1);  
  VALUE val;
  
  if (_value) {
	int len=amq_content_basic_get_body(content_obj->content,(byte*)_value,_size);
	if (len>=0)  {
	  val=rb_str_new(_value,len);
	} else {
	  val=Qnil;
	}
	free(_value);
  } else {
	rb_raise(rb_eNoMemError,"can't allocate receive buffer");
  }
  return val;
}


/*
 * AMQ_ContentBasic#body_size -> number
 */
static VALUE cont_content_basic_get_body_size(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  int size=amq_content_basic_get_body_size (content_obj->content);
  return INT2NUM(size);
}


/*
 *  AMQ_ContentBasic#exchange -> string
 *  the exchange where the content was published
 */
static VALUE cont_content_basic_get_exchange(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const exchange=amq_content_basic_get_exchange(content_obj->content);
  return rb_str_new2(exchange);
}




/*
 *  AMQ_ContentBasic#expiration -> string
 *  the exchange where the content was published
 */
static VALUE cont_content_basic_get_expiration(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const expiration=amq_content_basic_get_expiration(content_obj->content);
  return rb_str_new2(expiration);
}


static VALUE cont_content_basic_set_expiration(VALUE self,VALUE expiration)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(expiration,T_STRING);
  char *_expirat=StringValuePtr(expiration);
  amq_content_basic_set_expiration(content_obj->content,_expirat);
  return self;
}



static VALUE cont_content_basic_get_delivery_mode_persistent(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  if (amq_content_basic_get_delivery_mode(content_obj->content)==2)
	return Qtrue;
  else
	return Qfalse;
}


static VALUE cont_content_basic_set_delivery_mode_persistent(VALUE self,VALUE persist)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  int _persistent=VALUE2bool (persist,"arg: persistent not boolean");
  amq_content_basic_set_delivery_mode(content_obj->content,(_persistent)?2:1);
  return self;
}







/*
 *  AMQ_ContentBasic#headers -> hash
 */
static VALUE cont_content_basic_get_headers(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  icl_longstr_t *header=
	amq_content_basic_get_headers(content_obj->content);
  VALUE result=Qnil;
  result=create_amq_field_list_cls(asl_field_list_new (header));

  return result;
}



/* utility for headers= method*/


/*
 *  AMQ_ContentBasic#headers(hash) -> self
 */
static VALUE cont_content_basic_set_headers(VALUE self, VALUE headers)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);

  icl_longstr_t *header_arg= get_optional_field_list(headers);
  if (!header_arg) {
	rb_raise(rb_eArgError,"argument must be convertable to a AMQ_FieldList object");
  }

  amq_content_basic_set_headers(content_obj->content,header_arg);

  return self;
}



/*
  AMQ_ContentBasic#routing_key -> string
*/
static VALUE cont_content_basic_get_routing_key(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const routing_key=amq_content_basic_get_routing_key(content_obj->content);
  return rb_str_new2(routing_key);
}


/*
 * AMQ_ContentBasic#content_type -> string
 */
static VALUE cont_content_basic_get_content_type(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const content_type=amq_content_basic_get_content_type(content_obj->content);
  return rb_str_new2(content_type);
}

/*
 * AMQ_ContentBasic#content_type=(string) -> self
 */
static VALUE cont_content_basic_set_content_type(VALUE self,VALUE type)
{
  Check_Type(type,T_STRING);
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  char *_type = StringValuePtr(type);

  amq_content_basic_set_content_type(content_obj->content,_type);
  return self;
}




/*
 * AMQ_ContentBasic#content_type -> string
 * MIME Content-type
 */
static VALUE cont_content_basic_get_content_encoding(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const content_encoding=amq_content_basic_get_content_encoding(content_obj->content);
  return rb_str_new2(content_encoding);
}

/*
 * AMQ_ContentBasic#content_type=(string) -> self
 * MIME Content-type
 */
static VALUE cont_content_basic_set_content_encoding(VALUE self,VALUE encoding)
{
  Check_Type(encoding,T_STRING);
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  char *_encoding = StringValuePtr(encoding);

  amq_content_basic_set_content_encoding(content_obj->content,_encoding);
  return self;
}



/*
 * AMQ_ContentBasic#correlation_id -> string
 */
static VALUE cont_content_basic_get_correlation_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const corr_id=amq_content_basic_get_correlation_id(content_obj->content);
  return rb_str_new2(corr_id);
}


/*
 * AMQ_ContentBasic#correlation_id=(string) -> self
 */
static VALUE cont_content_basic_set_correlation_id(VALUE self,VALUE cor_id)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(cor_id,T_STRING);
  char *_cor_id = StringValuePtr(cor_id);

  amq_content_basic_set_correlation_id(content_obj->content,_cor_id);
  return self;
}



/*
 * AMQ_ContentBasic#message_id -> string
 *
 */
static VALUE cont_content_basic_get_message_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const message_id=amq_content_basic_get_message_id(content_obj->content);
  return rb_str_new2(message_id);
}

/*
 * AMQ_ContentBasic#message_id=(fixnum) -> self
 * AMQ_ContentBasic#message_id=(string) -> self
 *
 * thanks to Mitchell Ogle for the bug report
 */
static VALUE cont_content_basic_set_message_id(VALUE self, VALUE message_id)
{
  amq_content_cls * const content_obj=get_amq_content_cls(self,1);
  if( FIXNUM_P(message_id))
    { 
          icl_shortstr_t message_id_t;  
	  icl_shortstr_fmt (message_id_t, "ID%d", FIX2INT(message_id));
	  amq_content_basic_set_message_id (content_obj->content, message_id_t);
    } else if (TYPE(message_id) == T_STRING) {
         char *_message_id = StringValuePtr(message_id);
         if (!_message_id) {
	    rb_raise(rb_eTypeError,"message_id was a NULL string??");
         }
         amq_content_basic_set_message_id(content_obj->content,_message_id);
   } else {
	rb_raise(rb_eTypeError,"message_id was not a String nor a Fixnum");
  }
  return self;
}





/*
 * AMQ_ContentBasic#priority -> number
 */
static VALUE cont_content_basic_get_priority(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  int prio=amq_content_basic_get_priority(content_obj->content);
  return INT2NUM(prio);
}


/*
 * AMQ_ContentBasic#priority=(Number) -> self
 */
static VALUE cont_content_basic_set_priority(VALUE self,VALUE prio)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  int num=INT2NUM(prio);
  if (num<0 || num>0)
	rb_raise(rb_eArgError,"priority only in range 0..9");
  amq_content_basic_set_priority(content_obj->content,num);
  return self;
}


/*
 * AMQ_ContentBasic#producer_id -> string
 */
static VALUE cont_content_basic_get_producer_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const producer_id=amq_content_basic_get_producer_id(content_obj->content);
  return rb_str_new2(producer_id);
}



/*
 * AMQ_ContentBasic#reply_to -> string
 */
static VALUE cont_content_basic_get_reply_to(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const reply_to=amq_content_basic_get_reply_to(content_obj->content);
  return rb_str_new2(reply_to);
}


/*
 * AMQ_ContentBasic#reply_to=(string) -> self
 */
static VALUE cont_content_basic_set_reply_to(VALUE self,VALUE reply_to)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(reply_to,T_STRING);
  char *_reply_to = StringValuePtr(reply_to);

  amq_content_basic_set_reply_to(content_obj->content,_reply_to);
  return self;
}


/*
 * AMQ_ContentBasic#sender_id -> string
 *
 */
static VALUE cont_content_basic_get_sender_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const sender_id=amq_content_basic_get_sender_id(content_obj->content);
  return rb_str_new2(sender_id);
}

/*
 * AMQ_ContentBasic#sender_id=(string) -> self
 *
 */
static VALUE cont_content_basic_set_sender_id(VALUE self,VALUE sender_id)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(sender_id,T_STRING);
  char *_sender_id = StringValuePtr(sender_id);
  amq_content_basic_set_sender_id(content_obj->content,_sender_id);
  return self;
}

/*
 * AMQ_ContentBasic#type -> string
 *
 * message type
 */
static VALUE cont_content_basic_get_type(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const type=amq_content_basic_get_type(content_obj->content);
  return rb_str_new2(type);
}

/*
 * AMQ_ContentBasic#type=(string) -> self
 *
 * message type
 */
static VALUE cont_content_basic_set_type(VALUE self,VALUE type)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(type,T_STRING);
  char *_type = StringValuePtr(type);

  amq_content_basic_set_type(content_obj->content,_type);
  return self;
}



/*
 * AMQ_ContentBasic#user_id -> string
 *
 */
static VALUE cont_content_basic_get_user_id(VALUE self)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  const char *const user_id=amq_content_basic_get_user_id(content_obj->content);
  return rb_str_new2(user_id);
}

/*
 * AMQ_ContentBasic#user_id=(string) -> self
 *
 */
static VALUE cont_content_basic_set_user_id(VALUE self,VALUE user_id)
{
  amq_content_cls* const content_obj=get_amq_content_cls(self,1);
  Check_Type(user_id,T_STRING);
  char *_user_id = StringValuePtr(user_id);

  amq_content_basic_set_user_id(content_obj->content,_user_id);
  return self;
}

 
void init_content() {
  amqp_content = rb_define_class_under(rb_wireapi,"AMQ_ContentBasic",rb_cObject);
  //content
  //rb_define_method(amqp_content,"class_id",cont_content_basic_class_id,0);
  rb_define_method(amqp_content,"app_id",cont_content_basic_get_app_id,0);
  rb_define_method(amqp_content,"app_id=",cont_content_basic_set_app_id,1);
  rb_define_method(amqp_content,"body", cont_content_basic_get_body,-1);
  rb_define_method(amqp_content,"body=",cont_content_basic_set_body,1);
  rb_define_method(amqp_content,"body_size",cont_content_basic_get_body_size,0);
  rb_define_method(amqp_content,"content_encoding",cont_content_basic_get_content_encoding,0);
  rb_define_method(amqp_content,"content_encoding=",cont_content_basic_set_content_encoding,1);
  rb_define_method(amqp_content,"content_type",cont_content_basic_get_content_type,0);
  rb_define_method(amqp_content,"content_type=",cont_content_basic_set_content_type,1);
  rb_define_method(amqp_content,"correlation_id",cont_content_basic_get_correlation_id,0);
  rb_define_method(amqp_content,"correlation_id=",cont_content_basic_set_correlation_id,1);
  rb_define_method(amqp_content,"destroy",cont_content_basic_destroy,0);
  rb_define_method(amqp_content,"delivery_mode_persistent",cont_content_basic_get_delivery_mode_persistent,0);
  rb_define_method(amqp_content,"delivery_mode_persistent=",cont_content_basic_set_delivery_mode_persistent,1);

  rb_define_method(amqp_content,"exchange",cont_content_basic_get_exchange,0);
  rb_define_method(amqp_content,"expiration",cont_content_basic_get_expiration,0);
  rb_define_method(amqp_content,"expiration=",cont_content_basic_set_expiration,1);
  rb_define_method(amqp_content,"headers",cont_content_basic_get_headers,0);
  rb_define_method(amqp_content,"headers=",cont_content_basic_set_headers,1);
  rb_define_method(amqp_content,"message_id",cont_content_basic_get_message_id,0);
  rb_define_method(amqp_content,"message_id=",cont_content_basic_set_message_id,1);
  rb_define_method(amqp_content,"priority",cont_content_basic_get_priority,0);
  rb_define_method(amqp_content,"priority=",cont_content_basic_set_priority,1);
  rb_define_method(amqp_content,"producer_id",cont_content_basic_get_producer_id,0);
  rb_define_method(amqp_content,"reply_to",cont_content_basic_get_reply_to,0);
  rb_define_method(amqp_content,"reply_to=",cont_content_basic_set_reply_to,1);
  rb_define_method(amqp_content,"routing_key",cont_content_basic_get_routing_key,0);
  rb_define_method(amqp_content,"sender_id",cont_content_basic_get_sender_id,0);
  rb_define_method(amqp_content,"sender_id=",cont_content_basic_set_sender_id,1);
  rb_define_method(amqp_content,"type",cont_content_basic_get_type,0);
  rb_define_method(amqp_content,"type=",cont_content_basic_set_type,1);
  rb_define_method(amqp_content,"unlink",cont_content_basic_destroy,0);
  rb_define_method(amqp_content,"user_id",cont_content_basic_get_user_id,0);
  rb_define_method(amqp_content,"user_id=",cont_content_basic_set_user_id,1);
  rb_define_singleton_method(amqp_content, "new" , cont_content_basic_new , 0);



  //rb_define_method(amqp_content,"timestamp",cont_timestamp,0);
  //rb_define_method(amqp_content,"content_basic_set_class_id",cont_content_basic_set_class_id,0);
  //rb_define_method(amqp_content,"content_basic_set_timestamp",cont_content_basic_set_timestamp,0);
}
