#include "module.h"
#include <cassert>

napi_status get_string_value(napi_env env, napi_value args[], size_t index, char **value, size_t value_size)
{
 napi_status status;
 napi_valuetype value_type;
 size_t length=0;
 status=napi_typeof(env, args[index], &value_type);
 assert(status == napi_ok);

 if(value_type != napi_string)
 {
  return napi_string_expected;
 }

 status=napi_get_value_string_utf8(env, args[index], *value, value_size, &length);
 assert(status == napi_ok);
 ++length;
 if(length>value_size)
 {
  (*value)=static_cast<char *>(realloc(*value, length));
  assert((*value) != nullptr);
 }
 status=napi_get_value_string_utf8(env, args[index], *value, length, &length);
 assert(status == napi_ok);

 return napi_ok;
}

napi_ref Yolo::constructor;

Yolo::Yolo(char *darknet_path, char *datacfg, char *cfgfile, char *weightfile) : env_(nullptr), wrapper_(nullptr)
{
 yolo_init(darknet_path, datacfg, cfgfile, weightfile);
}

Yolo::~Yolo()
{
 napi_delete_reference(env_, wrapper_);
}

void Yolo::Destructor(napi_env env, void *nativeObject, void * /*finalize_hint*/)
{
 reinterpret_cast<Yolo *>(nativeObject)->~Yolo();
}

napi_value Yolo::Init(napi_env env, napi_value exports)
{
 napi_status status;
 napi_property_descriptor properties[]={{"detect", nullptr, Yolo::Detect, nullptr, nullptr, nullptr, napi_default, nullptr}};

 napi_value cons;
 status=napi_define_class(env, "Yolo", NAPI_AUTO_LENGTH, Yolo::New, nullptr, 1, properties, &cons);
 assert(status == napi_ok);

 status=napi_create_reference(env, cons, 1, &Yolo::constructor);
 assert(status == napi_ok);

 status=napi_set_named_property(env, exports, "Yolo", cons);
 assert(status == napi_ok);
 return exports;
}

napi_value Yolo::New(napi_env env, napi_callback_info info)
{
 napi_status status;

 napi_value target;
 status=napi_get_new_target(env, info, &target);
 assert(status == napi_ok);
 bool is_constructor=target != nullptr;

 if(is_constructor)
 {
  // Invoked as constructor: `new MyObject(...)`
  size_t argc=4;
  napi_value args[4];
  napi_value jsthis;
  status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
  assert(status == napi_ok);
  assert(argc>=4);
  char *darknet_path=nullptr;
  char *datacfg=nullptr;
  char *cfgfile=nullptr;
  char *weightfile=nullptr;

  get_string_value(env, args, 0, &darknet_path, 0);
  get_string_value(env, args, 1, &datacfg, 0);
  get_string_value(env, args, 2, &cfgfile, 0);
  get_string_value(env, args, 3, &weightfile, 0);

  Yolo *obj=new Yolo(darknet_path, datacfg, cfgfile, weightfile);

  obj->env_=env;
  status=napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), Yolo::Destructor, nullptr,  // finalize_hint
                   &obj->wrapper_);
  assert(status == napi_ok);

  return jsthis;
 }
 else
 {
  // Invoked as plain function `MyObject(...)`, turn into construct call.
  size_t argc_=4;
  napi_value args[4];
  status=napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
  assert(status == napi_ok);

  const size_t argc=4;
  napi_value argv[4]={args[0], args[1], args[2], args[3]};

  napi_value cons;
  status=napi_get_reference_value(env, Yolo::constructor, &cons);
  assert(status == napi_ok);

  napi_value instance;
  status=napi_new_instance(env, cons, argc, argv, &instance);
  assert(status == napi_ok);

  return instance;
 }
}

napi_value Yolo::Detect(napi_env env, napi_callback_info info)
{
 napi_status status;

 size_t argc=1;
 napi_value args[1];
 napi_value jsthis;
 status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
 assert(status == napi_ok);

 napi_value instance;
 status=napi_create_string_utf8(env, "hello", 6, &instance);
 assert(status == napi_ok);

 return instance;
}

NAPI_MODULE(NodeYoloJS, Yolo::Init);