#ifndef NODEYOLOJS_MODULE_H
#define NODEYOLOJS_MODULE_H

#include <node_api.h>
#include <cassert>
#include <libyolo.h>

class Yolo
{
public:
 static napi_value Init(napi_env env, napi_value exports);
 static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

 yolo_object *yolo;

private:
 explicit Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile);
 ~Yolo();
 static napi_value New(napi_env env, napi_callback_info info);
 static napi_value Detect(napi_env env, napi_callback_info info);
 static napi_ref constructor;

 napi_env env_;
 napi_ref wrapper_;
};

typedef struct
{
 yolo_object *yolo;
 char *image_path;
 napi_deferred deferred;
 napi_value return_value;
 napi_async_context context;
 napi_value resource;
}data_holder;

#endif