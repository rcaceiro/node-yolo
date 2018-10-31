#include "yolo_error.h"

yolo_status_detailed yolo_status_decode(yolo_status status)
{
 yolo_status_detailed status_detailed;
 status_detailed.error_code=status;
 switch(status)
 {
  case yolo_instanciation:
   status_detailed.error_message="Cannot instantiate due an error.";
   break;
  case yolo_cannot_realloc_detect:
   status_detailed.error_message="Cannot allocate detect in memory";
   break;
  case yolo_cannot_alloc_yolo_detection:
   status_detailed.error_message="Cannot allocate yolo_detection in memory";
   break;
  case yolo_cannot_alloc_node_yolo_object:
   status_detailed.error_message="Cannot allocate node_yolo_object in memory";
   break;
  case yolo_cannot_alloc_map:
   status_detailed.error_message="Cannot allocate map in memory";
   break;
  case yolo_cannot_change_to_working_dir:
   status_detailed.error_message="Cannot change to working directory";
   break;
  case yolo_object_is_not_initialized:
   status_detailed.error_message="yolo_object isn't allocated in memory";
   break;
  case yolo_working_dir_is_not_exists:
   status_detailed.error_message="working directory don't exists";
   break;
  case yolo_datacfg_is_not_exists:
   status_detailed.error_message="datacfg don't exists";
   break;
  case yolo_cfgfile_is_not_exists:
   status_detailed.error_message="cfgfile don't exists";
   break;
  case yolo_weight_file_is_not_exists:
   status_detailed.error_message="weight file don't exists";
   break;
  case yolo_working_dir_is_not_readable:
   status_detailed.error_message="working directory isn't readable";
   break;
  case yolo_datacfg_is_not_readable:
   status_detailed.error_message="datacfg isn't readable";
   break;
  case yolo_cfgfile_is_not_readable:
   status_detailed.error_message="cfgfile isn't readable";
   break;
  case yolo_weight_file_is_not_readable:
   status_detailed.error_message="weight file isn't readable";
   break;
  case yolo_names_file_is_not_exists:
   status_detailed.error_message="names file don't exists";
   break;
  case yolo_names_file_is_not_readable:
   status_detailed.error_message="names file isn't readable";
   break;
  case yolo_image_file_is_not_exists:
   status_detailed.error_message="image file isn't exists";
   break;
  case yolo_image_file_is_not_readable:
   status_detailed.error_message="image file isn't readable";
   break;
  case yolo_image_file_is_corrupted:
   status_detailed.error_message="image file is corrupted";
   break;
   //  case yolo_napi_create_object_time_spent_for_classification_double_failed:
   //   status_detailed.error_message="image file is corrupted";
   //   break;
   //  case yolo_napi_create_object_time_spent_for_classification_named_property_failed:
   //   status_detailed.error_message="image file is corrupted";
   //   break;
   //  case yolo_napi_set_array_property_failed:
   //   status_detailed.error_message="image file is corrupted";
   //   break;
   //  case yolo_napi_create_main_object_failed:
   //   status_detailed.error_message="image file is corrupted";
   //   break;
  default:
   status_detailed.error_message="Unknow error";
 }
 return status_detailed;
}