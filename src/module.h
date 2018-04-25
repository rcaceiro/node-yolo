#ifndef NODEYOLOJS_MODULE_H
#define NODEYOLOJS_MODULE_H

#include <node_api.h>
#include <cassert>
#include <libyolo.h>
#include <queue>
#include "Queue.h"

class Yolo
{
public:
 static napi_value Init(napi_env env, napi_value exports);
 static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
private:
 explicit Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile);
 ~Yolo();
 static napi_value New(napi_env env, napi_callback_info info);
 static napi_value Detect(napi_env env, napi_callback_info info);
 static napi_ref constructor;

 napi_env env_;
 napi_ref wrapper_;
 yolo_object *yolo;
 std::queue<char *> *queue_img_path;

};

typedef struct
{
 yolo_object *yolo;
 char *image_path;
 napi_deferred deferred;
 napi_async_work work;
 napi_value resource;
 yolo_detection *img_detection;
}data_holder;

#endif