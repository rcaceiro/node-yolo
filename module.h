#ifndef NODEYOLOJS_MODULE_H
#define NODEYOLOJS_MODULE_H

#include <node_api.h>
extern "C" {
#include "libyolo.h"
}
class Yolo
{
public:
 static napi_value Init(napi_env env, napi_value exports);

 static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
 explicit Yolo(char *darknet_path, char *datacfg, char *cfgfile, char *weightfile);

 ~Yolo();

 static napi_value New(napi_env env, napi_callback_info info);

 static napi_value Detect(napi_env env, napi_callback_info info);

 static napi_ref constructor;
 napi_env env_;
 napi_ref wrapper_;

 yolo_handle network;
};

#endif