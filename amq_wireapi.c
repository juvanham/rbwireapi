/*===========================================================================
 *
 *  OpenAMQ Ruby module
 *
 *  Copyright (c) 2008 Benjamin April
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *===========================================================================*/



#include "ruby.h"
#include "asl.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"
#include "amq_client_agent.h"
#include "amq_exchange.h"

#include <dlfcn.h>
#include <unistd.h>

VALUE rb_amq_error;
VALUE rb_amq_server_error;


// The order of these includes matters.
#include "amq_types.h"
#include "amq_content.h"
#include "amq_session.h"
#include "amq_connection.h"
#include "amq_field_list.h"

VALUE rb_wireapi;
VALUE amqp_auth;


static VALUE rbWireAPI_new(int argc, VALUE *argv,VALUE class)  {

  VALUE self=create_amq_wire_api_cls();
  rb_obj_call_init(self, argc, argv);

  return self;  
}


static VALUE rbWireAPI_init(int argc, VALUE *argv,VALUE self)

{
  VALUE trace;
  rb_scan_args(argc,argv,"01",&trace); 
  
  int opt_trace = 0;
  if  FIXNUM_P(trace) {
	  opt_trace=FIX2INT(trace) ;
	}
  
  if (opt_trace) {
	amq_client_connection_animate(opt_trace&4);
	amq_client_session_animate (opt_trace&8);
  }
  return (self);
}


static VALUE rbWireAPI_debug_connection(VALUE self, VALUE val)
{
  int connection_debug=VALUE2bool(val,"flag must be boolean");	
  amq_client_connection_animate (connection_debug);
  return (self);
}


static VALUE rbWireAPI_debug_session(VALUE self, VALUE val)
{
  int session_debug=VALUE2bool(val,"flag must be boolean");	
  amq_client_session_animate (session_debug);
  return (self);
}



static VALUE rbWireAPI_client_connection_auth_plain (VALUE self, VALUE username, VALUE password)
{
    icl_longstr_t   *auth_data;          
    VALUE           rb_auth_data;
    char *_username;
    char *_password;
    if (TYPE(username) == T_STRING)
        _username = StringValuePtr(username);

    else 
    {
        rb_raise(rb_eTypeError,"Username was not a string");
        return Qnil;
    }
    if (TYPE(password) == T_STRING)
        _password = StringValuePtr(password);

    else 
    {
        rb_raise(rb_eTypeError,"Password was not a string");
        return Qnil;
    }
    auth_data = amq_client_connection_auth_plain (_username, _password);
    rb_auth_data = create_amq_auth_cls(auth_data);
    return(rb_auth_data);
}

static VALUE rbWireAPI_client_connection_new(VALUE self, 
	VALUE host, 
	VALUE vhost,
	VALUE rb_auth_data, 
	VALUE client_name,
	VALUE trace, 
	VALUE timeout)
{
    icl_longstr_t *auth_data;
    amq_client_connection_t  *connection = NULL;  
    VALUE rb_connection;
    char *_hostname = StringValuePtr(host);
    char *_vhost = "/";
	amq_auth_cls  *auth_obj= get_amq_auth_cls(rb_auth_data,1);          
    if (TYPE(vhost)==T_STRING) {
      _vhost = StringValuePtr(vhost);
    }
    char *_client_name = StringValuePtr(client_name);
	int _trace=NUM2INT(trace) & 3; // trace (0-3)
	int _timeout=NUM2INT(timeout);
	  
    //  Open all connections
    Data_Get_Struct(rb_auth_data,icl_longstr_t,auth_data);
    connection = amq_client_connection_new (_hostname, _vhost, 
											 auth_obj->auth, 
											 _client_name, 
											 _trace, 
											 _timeout);
    if (!connection)
        rb_raise(rb_amq_error,"Could not connect to server");

    rb_connection =  create_amq_connection_cls(connection);
	if (rb_block_given_p()) {
	  rb_yield(rb_connection);
	  con_client_connection_destroy(rb_connection);
	  return Qtrue;
	} 
	return rb_connection;
}


void Init_WireAPI()
{
   rb_wireapi = rb_define_class("WireAPI",rb_cObject);
   amqp_auth = rb_define_class_under(rb_wireapi,"AMQ_Auth",rb_cObject);
   rb_amq_error= rb_define_class_under(rb_wireapi,"AMQ_Error",rb_eRuntimeError);
   rb_amq_server_error= rb_define_class_under(rb_wireapi,"AMQ_ServerError",rb_amq_error);

   // WireAPI
   rb_define_singleton_method(rb_wireapi, "new"   , 
							  rbWireAPI_new      , -1);

   rb_define_method(rb_wireapi,"initialize", 
					rbWireAPI_init,-1);

   rb_define_method(rb_wireapi,"auth_plain", 
					rbWireAPI_client_connection_auth_plain,2); 

   rb_define_method(rb_wireapi,"connection_new", 
					rbWireAPI_client_connection_new,6); 

   rb_define_method(rb_wireapi,"debug_connection=", 
					rbWireAPI_debug_connection,1); 

   rb_define_method(rb_wireapi,"debug_session=", 
					rbWireAPI_debug_session,1); 

   init_content();
   init_connection();
   init_session();
   init_field_list();
}
