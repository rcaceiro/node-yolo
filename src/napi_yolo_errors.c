#include <stddef.h>
#include "napi_yolo_errors.h"

void yolo_napi_fill_error_message(yolo_napi_status status, yolo_napi_status_detailed *status_detailed)
{
 switch(status)
 {
  case yolo_napi_create_object_time_spent_for_classification_double_failed:
   status_detailed->error_message="image file is corrupted";
   break;
  case yolo_napi_create_object_time_spent_for_classification_named_property_failed:
   status_detailed->error_message="image file is corrupted";
   break;
  case yolo_napi_set_array_property_failed:
   status_detailed->error_message="image file is corrupted";
   break;
  case yolo_napi_create_main_object_failed:
   status_detailed->error_message="image file is corrupted";
   break;
  default:
   status_detailed->error_message="Unknow error";
 }
}

yolo_napi_status_detailed yolo_napi_status_decode(yolo_napi_status status, yolo_status *status2)
{
 yolo_napi_status_detailed status_detailed;
 if(status == yolo_napi_error_from_libyolo)
 {
  if(status2 != NULL)
  {
   return yolo_napi_error_wrap(*status2);
  }
  else
  {
   status=yolo_napi_unknow_error;
  }
 }
 status_detailed.error_code=status;
 status_detailed.error_prefix="yolo_napi";
 yolo_napi_fill_error_message(status, &status_detailed);
 return status_detailed;
}

yolo_napi_status_detailed yolo_napi_error_wrap(yolo_status status)
{
 yolo_napi_status_detailed napi_status_detailed;
 yolo_status_detailed status_detailed=yolo_status_decode(status);
 napi_status_detailed.error_code=status_detailed.error_code;
 napi_status_detailed.error_message=status_detailed.error_message;
 napi_status_detailed.error_prefix="yolo";
 return napi_status_detailed;
}