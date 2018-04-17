let addons = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Debug/nodeyolojs');
let yolo = new addons.Yolo("./darknet", "./darknet/cfg/coco.data", "./darknet/cfg/yolov3.cfg", "yolov3.weights");
console.dir(yolo.detect());