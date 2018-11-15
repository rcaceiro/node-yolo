#ifndef NODE_YOLO_NAPI_YOLO_ERRORS_H
#define NODE_YOLO_NAPI_YOLO_ERRORS_H

#include "../yolo/src/yolo_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
 int error_code;
 char *error_prefix;
 char *error_message;
}yolo_napi_status_detailed;

typedef enum
{
 yolo_napi_ok,
 yolo_napi_create_main_object_failed,
 yolo_napi_create_array_failed,
 yolo_napi_set_array_property_failed,
 yolo_napi_create_object_failed,
 yolo_napi_set_object_to_array_failed,
 yolo_napi_create_class_name_string_failed,
 yolo_napi_set_class_name_property_failed,
 yolo_napi_create_probability_double_failed,
 yolo_napi_set_probability_property_failed,
 yolo_napi_create_box_object_failed,
 yolo_napi_set_box_property_failed,
 yolo_napi_create_frame_failed,
 yolo_napi_set_frame_to_object_failed,
 yolo_napi_create_second_failed,
 yolo_napi_set_second_to_object_failed,

 yolo_napi_create_box_x_double_failed,
 yolo_napi_create_box_x_named_property_failed,
 yolo_napi_create_box_y_double_failed,
 yolo_napi_create_box_y_named_property_failed,
 yolo_napi_create_box_w_double_failed,
 yolo_napi_create_box_w_named_property_failed,
 yolo_napi_create_box_h_double_failed,
 yolo_napi_create_box_h_named_property_failed,
 yolo_napi_create_object_time_spent_for_classification_double_failed,
 yolo_napi_create_object_time_spent_for_classification_named_property_failed,

 yolo_napi_error_from_libyolo,
 yolo_napi_unknow_error
}yolo_napi_status;

yolo_napi_status_detailed yolo_napi_status_decode(yolo_napi_status status, yolo_status *status2);

yolo_napi_status_detailed yolo_napi_error_wrap(yolo_status status);

#ifdef __cplusplus
}
#endif

#endif //NODE_YOLO_NAPI_YOLO_ERRORS_H