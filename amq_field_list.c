#include <ruby.h>
#include <st.h>
#include <string.h>
#include "amq_field_list.h"
#include "amq_types.h"
#include <limits.h>

/* used for converting float to fixpoint */
const long FIXPOINT_MAX=LONG_MAX/100;

VALUE amqp_field_list;

/* receives a AMQ_FieldList or a Hash */
icl_longstr_t* get_optional_field_list(VALUE val)  {
  amq_field_list_cls *obj=0;
  if (TYPE(val)==T_DATA && rb_obj_is_instance_of(val, amqp_field_list)) {
	obj=get_amq_field_list_cls(val,1);
  }
  if (TYPE(val)==T_HASH) {
	VALUE fld_list=rb_funcall(amqp_field_list,rb_intern("new"),1,val);
	obj= get_amq_field_list_cls(fld_list,1);
  }

  if (obj && obj->field_list) {
	return asl_field_list_flatten(obj->field_list);
  }
  return 0;
}


/* try to convert val to a string when it is not a string yet */
VALUE get_as_string(VALUE val) {
  if (TYPE(val)==T_STRING)
	return val;
  return rb_funcall(val,rb_intern("to_s"),0,0);
}




static VALUE field_value_to_ruby(asl_field_t *field) {
  switch (field->type) {
  case 'D': {
	long double val=field->integer;
	int d=field->decimals;
	while (--d>=0)
	  val/=10;
	return rb_float_new(val);
	break;
  }
  case 'T': {
	VALUE argv[1];
	argv[0]=LONG2NUM(asl_field_integer(field));
	VALUE time=rb_funcall2(rb_cTime,rb_intern("at"),1,argv);
	return time;
	break;
  }
  case 'S': {
	return rb_str_new2(asl_field_string(field));
	break;
  }
  case 'I': {
	return INT2NUM(asl_field_integer(field));
	break;
  }
  default:
	return Qnil;
  } 
}


static int add_ruby_value_to_field_list(asl_field_list_t *field_list,char *name,VALUE val) {
  int res=0;
  switch (TYPE(val)) {
  case T_NIL: 
	{
	  asl_field_new_void(field_list,name);
	  res=1;
	  break;
	}
  case T_DATA: 
	{
	  if (rb_obj_is_instance_of(val, rb_cTime)) {
		VALUE _time=rb_funcall(val,rb_intern("tv_sec"),0,0);
		long t=NUM2LONG(_time);
		asl_field_new_time(field_list,name,t);
		res=1;
		break;
	  }
	  
	}
  case T_FLOAT:
	{
	  long double v=NUM2DBL(val);
	  int dec=0;
	  // make sure the integer doesn't roll over
	  while ((((long)v)-v)!=0 && abs(v)<FIXPOINT_MAX && dec<255) {
		v*=10;
		dec++;
	  }
	  asl_field_new_decimal(field_list,name,(long)v,dec);	  
	  res=1;
	  break;
	}
  case T_STRING: 
	{
	  char *str_val= StringValuePtr(val);
	  asl_field_new_string(field_list,name,str_val);
	  res=1;
	  break;
	}
  case T_FIXNUM: 
	{
	  long int_val= NUM2LONG(val);
	  asl_field_new_integer(field_list,name,int_val);
	  res=1;
	  break;
	}
  }
  return res;
}


static VALUE field_list_new(int argc,VALUE *argv,VALUE klass)
{
  VALUE self=create_amq_field_list_cls(asl_field_list_new (0));
  rb_obj_call_init(self, argc, argv);
  return self;
}


static VALUE field_list_initialize(int argc,VALUE *argv,VALUE self)
{
  VALUE param;
  rb_scan_args(argc,argv,"01",&param);
  if (param==Qnil) {
	return self;
  }

  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  if (TYPE(param)==T_HASH) {
	return rb_funcall(self,rb_intern("add_hash"),1,param);
  }  
  if (TYPE(param)==T_DATA && rb_obj_is_instance_of(param, amqp_field_list)) {
	amq_field_list_cls *obj=get_amq_field_list_cls(param,1);
	asl_field_t *field = 0;
	field=asl_field_list_first (obj->field_list);
	while (field) {
	  VALUE val_arg=field_value_to_ruby(field);
	  add_ruby_value_to_field_list(fieldlst->field_list,field->name,val_arg);
	  field = asl_field_list_next (&field);
	}
	return self;
  }  
  return self;
}


static VALUE field_list_destroy(VALUE self)
{
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  if (fieldlst->field_list) {
	asl_field_list_destroy(&(fieldlst->field_list));
    fieldlst->field_list=0;
  }
  return Qnil;
}


static VALUE field_list_to_hash(VALUE self) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  VALUE result=rb_hash_new();
  VALUE key;
  VALUE val;
  asl_field_t *field = 0;
  field=asl_field_list_first (fieldlst->field_list);
  while (field) {
	key=rb_str_new2(field->name);
	val=field_value_to_ruby(field);
	rb_hash_aset(result,key,val);
	field = asl_field_list_next (&field);
  }
  return result;
}

static VALUE field_list_each_pair(VALUE self) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  VALUE result=rb_hash_new();
  VALUE key_arg;
  VALUE val_arg;
  asl_field_t *field = 0;
  field=asl_field_list_first (fieldlst->field_list);

  while (field) {
	key_arg=rb_str_new2(field->name);
	val_arg=field_value_to_ruby(field);
	if (rb_block_given_p()==Qtrue) {
	  rb_yield_values(2,key_arg,val_arg);		 
	} else {
	  rb_raise(rb_eArgError, "need block with 2 arguments");
	}	
	field = asl_field_list_next (&field);
  }
  return result;
}




static VALUE field_list_get_key(VALUE self,VALUE name_val) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  VALUE name=get_as_string(name_val);
  Check_Type(name,T_STRING);
  char *_name = StringValuePtr(name);
  asl_field_t *field = 0;
  field=asl_field_list_first (fieldlst->field_list);
  VALUE result=Qnil;
  while (field && result==Qnil) {
	if (strcmp(field->name,_name)==0) {
	  result=field_value_to_ruby(field);
	}
	field = asl_field_list_next (&field);
  }
  return result;
}



static VALUE field_list_put_key(VALUE self,VALUE name_val,VALUE val) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  VALUE name=get_as_string(name_val);
  Check_Type(name,T_STRING);
  char *_name = StringValuePtr(name);  
  int res=add_ruby_value_to_field_list(fieldlst->field_list,_name,val);
  if (res==0) {
	rb_raise(rb_amq_error,"Can't handle value type");
  }
  return self;
}



/* utility */
static int push_hash_key(VALUE key, VALUE value, VALUE buf) {
  rb_ary_push(buf,key);
  return ST_CONTINUE;
}


static VALUE field_list_keys(VALUE self) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  VALUE result=rb_ary_new();
  asl_field_t *field = 0;
  field=asl_field_list_first(fieldlst->field_list);
  while (field) {
	VALUE key_str=rb_str_new2(field->name);
	rb_ary_push(result,key_str);
	field = asl_field_list_next (&field);
  }
  return result;
}


static VALUE field_list_keys_unique(VALUE self) {
  VALUE key_array=field_list_keys(self);  
  ID include_method=rb_intern("include?");
  
  while (RARRAY_LEN(key_array)>0) {
	VALUE elem=rb_ary_pop(key_array);
	if (Qtrue==rb_funcall(key_array,include_method,1,elem)) {
	  return Qfalse;
	}
  } 
  return Qtrue;
}


static VALUE field_list_add_hash(VALUE self,VALUE hash) {
  amq_field_list_cls* const fieldlst=get_amq_field_list_cls(self,1);
  Check_Type(hash,T_HASH);
  VALUE key_arr=rb_ary_new();
  rb_hash_foreach(hash, push_hash_key, key_arr);
  
  VALUE key=rb_ary_shift(key_arr);
  while (key!=Qnil) {
	VALUE key_name=get_as_string(key);
	char *_key = StringValuePtr(key_name);
	VALUE val=rb_hash_aref(hash,key);
	int res=add_ruby_value_to_field_list(fieldlst->field_list,_key,val);
	if (res==0) {
	  rb_raise(rb_amq_error,"Can't handle value type for ",key);
	}	  
	key=rb_ary_shift(key_arr);	
  }
  return self;
}





void init_field_list() {
  amqp_field_list = rb_define_class_under(rb_wireapi,"AMQ_FieldList",rb_cObject); 
  rb_define_singleton_method(amqp_field_list , "new", field_list_new , -1);
  rb_define_method(amqp_field_list,"initialize"  , field_list_initialize ,-1);
  rb_define_method(amqp_field_list,"to_hash"     , field_list_to_hash ,0);
  rb_define_method(amqp_field_list,"keys"        , field_list_keys    ,0);
  rb_define_method(amqp_field_list,"destroy"     , field_list_destroy ,0);
  rb_define_method(amqp_field_list,"each_pair"   , field_list_each_pair,0);
  rb_define_method(amqp_field_list,"[]"          , field_list_get_key ,1);
  rb_define_method(amqp_field_list,"get"         , field_list_get_key ,1);
  rb_define_method(amqp_field_list,"put"         , field_list_put_key ,2);
  rb_define_method(amqp_field_list,"[]="         , field_list_put_key ,2);
  rb_define_method(amqp_field_list,"add_hash"    , field_list_add_hash,1);
  rb_define_method(amqp_field_list,"keys_unique?", field_list_keys_unique,0);

}

