
#include <ruby.h>
#include "amq_types.h"



/*  session allocator, destructor , accessor */


amq_session_cls* get_amq_session_cls(VALUE rb_obj,int raise) {
  amq_session_cls *result=0;
  Data_Get_Struct(rb_obj,amq_session_cls,result);
  if (raise && !result->session) {
    rb_raise(rb_amq_error,"Session already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, amqp_session)) {
	rb_raise(rb_eTypeError, "expected an AMQ_Session object");
  }
  return result;
}

void free_amq_session_cls( amq_session_cls *obj) {
  if (obj && obj->session) {
    amq_client_session_destroy (&(obj->session));
    obj->session=0;
  }
  if (obj)
    free(obj);
}

VALUE create_amq_session_cls(amq_client_session_t  *session) {
  VALUE rb_session;
  amq_session_cls *obj=ALLOC(amq_session_cls);
  obj->session=session;    
  rb_session = Data_Wrap_Struct(amqp_session,0,free_amq_session_cls,obj);  
  return rb_session;  
}


/*  content allocator, destructor , accessor */
amq_content_cls* get_amq_content_cls(VALUE rb_obj,int raise) {
  amq_content_cls *result=0;
  Data_Get_Struct(rb_obj,amq_content_cls,result);
  if (raise && !result->content) {
    rb_raise(rb_amq_error,"Content already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, amqp_content)) {
	rb_raise(rb_eTypeError, "expected an AMQ_ContentBasic object");
  }
  return result;
}

void free_amq_content_cls( amq_content_cls *obj) {
  if (obj && obj->content) {
    amq_content_basic_unlink(&(obj->content));
    obj->content=0;
  }
  if (obj)
    free(obj);
}

VALUE create_amq_content_cls(amq_content_basic_t  *content) {
  VALUE rb_content;
  amq_content_cls *obj=ALLOC(amq_content_cls);
  obj->content=content;    
  rb_content = Data_Wrap_Struct(amqp_content,0,free_amq_content_cls,obj);  
  return rb_content;  
}
 

/*  connection allocator, destructor , accessor */

amq_connection_cls* get_amq_connection_cls(VALUE rb_obj,int raise) {
  amq_connection_cls *result=0;
  Data_Get_Struct(rb_obj,amq_connection_cls,result);
  if (raise && !result->connection) {
    rb_raise(rb_amq_error,"Connection already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, amqp_connection)) {
	rb_raise(rb_eTypeError, "expected an AMQ_Connection object");
  }
  return result;
}

void free_amq_connection_cls( amq_connection_cls *obj) {
  if (obj && obj->connection) {
    amq_client_connection_destroy(&(obj->connection));
    obj->connection=0;
  }
  if (obj)
    free(obj);
}

VALUE create_amq_connection_cls(amq_client_connection_t  *connection) {
  VALUE rb_connection;
  amq_connection_cls *obj=ALLOC(amq_connection_cls);
  obj->connection=connection;    
  rb_connection = Data_Wrap_Struct(amqp_connection,0,free_amq_connection_cls,obj);  
  return rb_connection;  
}
 

 

/*  connection allocator, destructor , accessor */

amq_auth_cls* get_amq_auth_cls(VALUE rb_obj,VALUE raise) {
  amq_auth_cls *result=0;
  Data_Get_Struct(rb_obj,amq_auth_cls,result);
  if (raise && !result->auth) {
    rb_raise(rb_amq_error,"Auth_data already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, amqp_auth)) {
	rb_raise(rb_eTypeError, "expected an AMQ_Auth object");
  }
  return result;
}


void free_amq_auth_cls( amq_auth_cls *obj) {
  if (obj && obj->auth) {
	icl_longstr_destroy(&(obj->auth));
    obj->auth=0;
  }
  if (obj)
    free(obj);
}


VALUE create_amq_auth_cls(icl_longstr_t *auth) {
  VALUE rb_auth;
  amq_auth_cls *obj=ALLOC(amq_auth_cls);
  obj->auth=auth;    
  rb_auth = Data_Wrap_Struct(amqp_auth,0,free_amq_auth_cls,obj);  
  return rb_auth;  
}
 

/*  int6_t allocator, destructor , accessor */
amq_int64_cls* get_amq_int64_cls(VALUE rb_obj) {
  amq_int64_cls *result=0;
  if (!rb_obj_is_instance_of(rb_obj, amqp_int64)) {
	rb_raise(rb_eTypeError, "expected an AMQ_int64 object");
  }
  Data_Get_Struct(rb_obj,amq_int64_cls,result);
  return result;
}

void free_amq_int64_cls( amq_int64_cls *obj) {
  if (obj && obj->value) {
    obj->value=0;
  }
  if (obj)
    free(obj);
}

VALUE create_amq_int64_cls(int64_t int64) {
  VALUE rb_int64;
  amq_int64_cls *obj=ALLOC(amq_int64_cls);
  obj->value=int64;    
  rb_int64 = Data_Wrap_Struct(amqp_int64,0,free_amq_int64_cls,obj);  
  return rb_int64;  
}
 

int VALUE2bool (VALUE v,const char *msg) {
  if (v==Qtrue)
	return 1;
  if (v==Qfalse)
	return 0;
  char total_msg[250];
  if (TYPE(v)!= T_ICLASS) {
	sprintf(total_msg,"%s (%x)",msg,TYPE(v));
  } else {
	sprintf(total_msg,"%s (%s)",msg,rb_obj_classname(v));

  }
  
  rb_raise(rb_amq_error,total_msg);
  return 0;
}











 

/*  connection allocator, destructor , accessor */

amq_field_list_cls* get_amq_field_list_cls(VALUE rb_obj,VALUE raise) {
  amq_field_list_cls *result=0;
  Data_Get_Struct(rb_obj,amq_field_list_cls,result);
  if (raise && !result->field_list) {
    rb_raise(rb_amq_error,"field_list already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, amqp_field_list)) {
	rb_raise(rb_eTypeError, "expected an AMQ_FieldList");
  }

  return result;
}


void free_amq_field_list_cls( amq_field_list_cls *obj) {
  if (obj && obj->field_list) {
	asl_field_list_destroy (&(obj->field_list));
    obj->field_list=0;
  }
  if (obj)
    free(obj);
}


VALUE create_amq_field_list_cls(asl_field_list_t *field_list) {
  VALUE rb_field_list;
  amq_field_list_cls *obj=ALLOC(amq_field_list_cls);
  obj->field_list=field_list;    
  rb_field_list = Data_Wrap_Struct(amqp_field_list,0,free_amq_field_list_cls,obj);  
  return rb_field_list;  
}




/*  rb_wireapi allocator, destructor , accessor */
static int wire_api_ref_counter=0;
static int wire_api_instance_counter=0;

amq_wire_api_cls* get_amq_wire_api_cls(VALUE rb_obj,VALUE raise) {
  amq_wire_api_cls *result=0;
  Data_Get_Struct(rb_obj,amq_wire_api_cls,result);
  if (raise && !result) {
    rb_raise(rb_amq_error,"WireAPI already destroyed");
  }
  if (!rb_obj_is_instance_of(rb_obj, rb_wireapi)) {
	rb_raise(rb_eTypeError, "expected an WireAPI object");
  }
  return result;
}


void free_amq_wire_api_cls( amq_wire_api_cls *obj) {
  if (obj && obj->instance_id) {
    obj->instance_id=0;
	--wire_api_ref_counter;
	if (wire_api_ref_counter<=0) {
	  icl_system_terminate ();
	}
  }
  if (obj)
    free(obj);
}


VALUE create_amq_wire_api_cls() {
  VALUE rb_wapi;
  amq_wire_api_cls *obj=ALLOC(amq_wire_api_cls);
  //  obj->field_list=field_list;    
  rb_wapi = Data_Wrap_Struct(rb_wireapi,0,free_amq_wire_api_cls,obj);  
  if (wire_api_ref_counter==0) {
	icl_system_initialise (0,0);
  }
  ++wire_api_ref_counter;
  obj->instance_id=++wire_api_instance_counter;
  return rb_wapi;
}
