#ifndef NODE_YOLO_MODULE_H
#define NODE_YOLO_MODULE_H

#include <node_api.h>
#include <libyolo.h>

class Yolo
{
public:
 static napi_value Init(napi_env env, napi_value exports);

 static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

 void mutex_lock();
 void mutex_unlock();
 yolo_object *yolo;
 bool created;
private:
 explicit Yolo(char *working_directory, char *datacfg, char *cfgfile, char *weightfile);
 ~Yolo();

 static napi_value New(napi_env env, napi_callback_info info);

 static napi_value DetectImage(napi_env env, napi_callback_info info);
 static napi_value DetectVideo(napi_env env, napi_callback_info info);

 static napi_ref constructor;

 pthread_mutex_t mutex;
 napi_env env_;
 napi_ref wrapper_;
};

typedef struct
{
 Yolo *yolo;
 char *image_path;
 napi_deferred deferred;
 napi_async_work work;
 napi_value resource;
 yolo_detection *img_detection;
 yolo_status yolo_stats;
}data_holder;

#endif