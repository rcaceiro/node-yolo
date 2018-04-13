let addons = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Debug/nodeyolojs');
let yolo = new addons.Yolo("./darknet", "cfg/coco.data", "cfg/tiny-yolo.cfg", "../tiny-yolo.weights");
console.dir(yolo.detect());