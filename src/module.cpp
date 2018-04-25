#include "module.h"

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

void load_box_object(napi_env env, box img_box, napi_value jsbox)
{
 napi_status status;
 napi_value x, y, w, h;
 status=napi_create_double(env, img_box.x, &x);
 assert(status == napi_ok);
 status=napi_set_named_property(env, jsbox, "x", x);
 assert(status == napi_ok);
 status=napi_create_double(env, img_box.y, &y);
 assert(status == napi_ok);
 status=napi_set_named_property(env, jsbox, "y", y);
 assert(status == napi_ok);
 status=napi_create_double(env, img_box.w, &w);
 assert(status == napi_ok);
 status=napi_set_named_property(env, jsbox, "w", w);
 assert(status == napi_ok);
 status=napi_create_double(env, img_box.h, &h);
 assert(status == napi_ok);
 status=napi_set_named_property(env, jsbox, "h", h);
 assert(status == napi_ok);
}

void load_detections(napi_env env, yolo_detection *img_detections, napi_value jsarray)
{
 napi_status status;
 napi_value jsobj, box_object, classes, prob, objectness, sort_class;
 detection detect;
 for(int i=0; i<img_detections->num_boxes; ++i)
 {
  detect=img_detections->detection[i];
  status=napi_create_object(env, &jsobj);
  assert(status == napi_ok);
  status=napi_set_element(env, jsarray, (uint32_t)i, jsobj);
  assert(status == napi_ok);
  status=napi_create_int32(env, detect.classes, &classes);
  assert(status == napi_ok);
  status=napi_set_named_property(env, jsobj, "classes", classes);
  assert(status == napi_ok);
  status=napi_create_int32(env, detect.sort_class, &sort_class);
  assert(status == napi_ok);
  status=napi_set_named_property(env, jsobj, "sort_class", sort_class);
  assert(status == napi_ok);
  status=napi_create_double(env, detect.objectness, &objectness);
  assert(status == napi_ok);
  status=napi_set_named_property(env, jsobj, "objectness", objectness);
  assert(status == napi_ok);
  status=napi_create_object(env, &box_object);
  assert(status == napi_ok);
  status=napi_set_named_property(env, jsobj, "box", box_object);
  assert(status == napi_ok);
  load_box_object(env, detect.bbox, box_object);
  if(detect.prob != nullptr)
  {
   status=napi_create_double(env, *detect.prob, &prob);
  }
  else
  {
   status=napi_get_undefined(env, &prob);
  }
  assert(status == napi_ok);
  status=napi_set_named_property(env, jsobj, "prob", prob);
  assert(status == napi_ok);
 }
}

void complete_async_detect(napi_env env, napi_status status, void *data)
{
 auto *holder=static_cast<data_holder *>(data);

 napi_value instance;
 status=napi_create_object(env, &instance);
 assert(status == napi_ok);
 napi_value yolo_detections_number, yolo_detections;
 status=napi_create_int32(env, holder->img_detection->num_boxes, &yolo_detections_number);
 assert(status == napi_ok);
 status=napi_create_array_with_length(env, (size_t)holder->img_detection->num_boxes, &yolo_detections);
 assert(status == napi_ok);
 status=napi_set_named_property(env, instance, "detections_number", yolo_detections_number);
 assert(status == napi_ok);
 status=napi_set_named_property(env, instance, "detections", yolo_detections);
 assert(status == napi_ok);
 load_detections(env, holder->img_detection, yolo_detections);

 napi_resolve_deferred(env, holder->deferred, instance);

 yolo_detection_free(&holder->img_detection);
 napi_async_work work=holder->work;

 free(holder->image_path);
 free(holder);

 napi_delete_async_work(env, work);
}

void async_detect(napi_env env, void *data)
{
 auto *holder=static_cast<data_holder *>(data);
 holder->img_detection=yolo_detect(holder->yolo, holder->image_path, 0.50);
}

napi_ref Yolo::constructor;

Yolo::Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile) : env_(nullptr), wrapper_(nullptr)
{
 this->yolo=yolo_init(working_directory, datacfg, cfgfile, weightfile);
}

Yolo::~Yolo()
{
 yolo_cleanup(this->yolo);
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

  auto *obj=new Yolo(darknet_path, datacfg, cfgfile, weightfile);

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
 napi_deferred deferred;
 napi_value promise;
 napi_value jsthis;
 size_t argc=1;
 napi_value args[1];

 status=napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
 assert(status == napi_ok);
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
 holder->yolo=yolo_obj->yolo;
 holder->resource=resource;

 status=napi_create_async_work(env, resource, resource_name, async_detect, complete_async_detect, holder, &holder->work);
 assert(status == napi_ok);
 status=napi_queue_async_work(env, holder->work);
 assert(status == napi_ok);
 return promise;
}

NAPI_MODULE(NodeYoloJS, Yolo::Init);