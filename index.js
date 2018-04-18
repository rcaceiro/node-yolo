let addons = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Debug/nodeyolojs');
let yolo = new addons.Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov3.cfg", "../yolov3.weights");
console.log(yolo.detect("darknet/data/dog.jpg"));