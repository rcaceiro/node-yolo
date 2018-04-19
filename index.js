let addons = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Debug/nodeyolojs');
let yolo = new addons.Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov3.cfg", "../yolov3-416.weights");
// let yolo1 = new addons.Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov2-tiny.cfg", "../yolov2-tiny.weights");
let obj = yolo.detect("darknet/data/kite.jpg");
console.log(JSON.stringify(obj));
// let obj1=yolo1.detect("darknet/data/kite.jpg");
// console.log(JSON.stringify(obj1));