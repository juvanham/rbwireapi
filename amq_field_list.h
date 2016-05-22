
#ifndef AMQ_FIELD_LIST_HEADER
#define AMQ_FIELD_LIST_HEADER

#include <ruby.h>
#include <icl.h>

icl_longstr_t* get_optional_field_list(VALUE val);

void init_field_list();


#endif
