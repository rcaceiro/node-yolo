#ifndef NODE_YOLO_YOLO_ERROR_H
#define NODE_YOLO_YOLO_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
 int error_code;
 char *error_message;
}yolo_status_detailed;

typedef enum
{
 yolo_ok,
 yolo_instanciation,
 yolo_cannot_alloc_node_yolo_object,
 yolo_cannot_alloc_map,
 yolo_cannot_alloc_yolo_detection,
 yolo_cannot_realloc_detect,
 yolo_cannot_change_to_working_dir,
 yolo_object_is_not_initialized,
 yolo_working_dir_is_not_exists,
 yolo_datacfg_is_not_exists,
 yolo_cfgfile_is_not_exists,
 yolo_weight_file_is_not_exists,
 yolo_working_dir_is_not_readable,
 yolo_datacfg_is_not_readable,
 yolo_cfgfile_is_not_readable,
 yolo_weight_file_is_not_readable,
 yolo_names_file_is_not_exists,
 yolo_names_file_is_not_readable,
 yolo_image_file_is_not_exists,
 yolo_image_file_is_not_readable,
 yolo_image_file_is_corrupted,

 yolo_video_cannot_alloc_base_structure,
 yolo_cannot_open_video_stream,

 yolo_unknow_error
}yolo_status;

yolo_status_detailed yolo_status_decode(yolo_status status);

#ifdef __cplusplus
}
#endif

#endif //NODE_YOLO_YOLO_ERROR_H
