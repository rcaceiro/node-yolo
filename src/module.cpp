#include "module.h"
#include <cassert>

napi_status get_string_value(napi_env env, napi_value args[], size_t index, char **value, size_t value_size)
{
 napi_status status;
 napi_valuetype value_type;
 size_t length=0;
 status=napi_typeof(env, args[index], &value_type);
 if(status != napi_ok)
 {
  return napi_invalid_arg;
 }

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

yolo_status load_box_object(napi_env env, box img_box, napi_value jsbox)
{
 napi_status status;
 napi_value x, y, w, h;

 status=napi_create_double(env, img_box.x, &x);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_x_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "x", x);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_x_named_property_failed;
 }

 status=napi_create_double(env, img_box.y, &y);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_y_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "y", y);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_y_named_property_failed;
 }

 status=napi_create_double(env, img_box.w, &w);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_w_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "w", w);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_w_named_property_failed;
 }

 status=napi_create_double(env, img_box.h, &h);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_h_double_failed;
 }
 status=napi_set_named_property(env, jsbox, "h", h);
 if(status != napi_ok)
 {
  return yolo_napi_create_box_h_named_property_failed;
 }
 return yolo_ok;
}

yolo_status load_detections(napi_env env, yolo_detection *img_detections, napi_value jsarray)
{
 napi_status status;
 napi_value jsobj, box_object, classes, prob;
 detect det;
 for(int i=0; i<img_detections->num_boxes; ++i)
 {
  det=img_detections->detection[i];
  status=napi_create_object(env, &jsobj);
  if(status != napi_ok)
  {
   return yolo_napi_create_object_failed;
  }

  status=napi_create_string_utf8(env, det.class_name, strlen(det.class_name), &classes);
  if(status != napi_ok)
  {
   return yolo_napi_create_class_name_string_failed;
  }
  status=napi_set_named_property(env, jsobj, "className", classes);
  if(status != napi_ok)
  {
   return yolo_napi_set_class_name_property_failed;
  }

  status=napi_create_double(env, det.probability, &prob);
  if(status != napi_ok)
  {
   return yolo_napi_create_probability_double_failed;
  }
  status=napi_set_named_property(env, jsobj, "probability", prob);
  if(status != napi_ok)
  {
   return yolo_napi_set_probability_property_failed;
  }

  status=napi_create_object(env, &box_object);
  if(status != napi_ok)
  {
   return yolo_napi_create_box_object_failed;
  }
  status=napi_set_named_property(env, jsobj, "box", box_object);
  if(status != napi_ok)
  {
   return yolo_napi_set_box_property_failed;
  }

  if(load_box_object(env, det.bbox, box_object) == yolo_ok)
  {
   status=napi_set_element(env, jsarray, (uint32_t)i, jsobj);
   if(status != napi_ok)
   {
    return yolo_napi_set_object_to_array_failed;
   }
  }
 }
 return yolo_ok;
}

void complete_async_detect(napi_env env, napi_status status, void *data)
{
 auto *holder=static_cast<data_holder *>(data);
 napi_value instance;
 yolo_status yolo_stats=yolo_ok;
 status=napi_create_array_with_length(env, (size_t)holder->img_detection->num_boxes, &instance);
 if(status != napi_ok)
 {
  yolo_stats=yolo_napi_create_array_failed;
 }

 if(yolo_stats == yolo_ok)
 {
  yolo_stats=load_detections(env, holder->img_detection, instance);
 }

 if(yolo_stats == yolo_ok)
 {
  napi_resolve_deferred(env, holder->deferred, instance);
 }
 else
 {
  napi_value error;
  yolo_status_detailed status_detailed=yolo_status_decode(yolo_stats);
  if(napi_create_object(env, &error) == napi_ok)
  {
   napi_value string_error;
   if(napi_create_string_utf8(env, status_detailed.error_message, strlen(status_detailed.error_message), &string_error) == napi_ok)
   {
    napi_set_named_property(env, error, "errorMessage", string_error);
   }
   napi_value error_code;
   if(napi_create_int32(env, status_detailed.error_code, &error_code) == napi_ok)
   {
    napi_set_named_property(env, error, "errorCode", error_code);
   }
  }
  napi_reject_deferred(env, holder->deferred, error);
 }

 yolo_detection_free(&holder->img_detection);
 napi_async_work work=holder->work;

 free(holder->image_path);
 free(holder);

 napi_delete_async_work(env, work);
}

void async_detect(napi_env env, void *data)
{
 (void)env;
 auto *holder=static_cast<data_holder *>(data);
 if(holder->yolo->created)
 {
  holder->yolo->mutex_lock();
  holder->yolo_stats=yolo_detect(holder->yolo->yolo, &holder->img_detection, holder->image_path, 0.50);
  holder->yolo->mutex_unlock();
 }
 else
 {
  holder->yolo_stats=yolo_instanciation;
 }
}

napi_ref Yolo::constructor;

Yolo::Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile) : env_(nullptr), wrapper_(nullptr)
{
 this->yolo=nullptr;
 yolo_status yolo_stats=yolo_init(&this->yolo, working_directory, datacfg, cfgfile, weightfile);
 if(yolo_stats != yolo_ok)
 {
  this->created=false;
  return;
 }
 pthread_mutex_init(&this->mutex, nullptr);
 this->created=true;
}

Yolo::~Yolo()
{
 pthread_mutex_destroy(&this->mutex);
 yolo_cleanup(this->yolo);
 napi_delete_reference(env_, wrapper_);
}

void Yolo::Destructor(napi_env env, void *nativeObject, void * /*finalize_hint*/)
{
 (void)env;
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

  auto obj=new Yolo(darknet_path, datacfg, cfgfile, weightfile);
  obj->env_=env;
  status=napi_wrap(env, jsthis, reinterpret_cast<void *>(obj), Yolo::Destructor, nullptr, &obj->wrapper_);
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
 napi_deferred deferred;
 napi_value promise;
 napi_value jsthis;
 size_t argc=1;
 napi_value args[1];

 status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
 if(status != napi_ok)
 {
  napi_throw_error(env, "00", "Cannot get arguments");
  return nullptr;
 }

 if(argc<1)
 {
  napi_throw_error(env, "01", "You have to pass the path to image as parameter");
  return nullptr;
 }

 char *image_path=nullptr;
 status=get_string_value(env, args, 0, &image_path, 0);
 assert(status == napi_ok);
 if(image_path == nullptr)
 {
  napi_throw_error(env, "02", "Cannot get image path");
  return nullptr;
 }

 void *obj=nullptr;
 status=napi_unwrap(env, jsthis, &obj);
 assert(status == napi_ok);
 auto *yolo_obj=static_cast<Yolo *>(obj);

 napi_value resource_name;
 napi_value resource;
 status=napi_create_string_utf8(env, "nodeyolojs.detect", 18, &resource_name);
 assert(status == napi_ok);
 status=napi_create_object(env, &resource);
 assert(status == napi_ok);

 auto *holder=static_cast<data_holder *>(calloc(1, sizeof(data_holder)));
 if(holder == nullptr)
 {
  napi_throw_error(env, "03", "Cannot allocate a struct in memory");
  return nullptr;
 }

 status=napi_create_promise(env, &deferred, &promise);
 assert(status == napi_ok);

 holder->deferred=deferred;
 holder->image_path=image_path;
 holder->yolo=yolo_obj;
 holder->resource=resource;

 status=napi_create_async_work(env, resource, resource_name, async_detect, complete_async_detect, holder, &holder->work);
 assert(status == napi_ok);
 status=napi_queue_async_work(env, holder->work);
 assert(status == napi_ok);

 return promise;
}

void Yolo::mutex_lock()
{
 pthread_mutex_lock(&this->mutex);
}

void Yolo::mutex_unlock()
{
 pthread_mutex_unlock(&this->mutex);
}

NAPI_MODULE(NodeYolo, Yolo::Init);