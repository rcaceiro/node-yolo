let addons = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Release/nodeyolojs');
let yolo = new addons.Yolo("./darknet", "./darknet/cfg/coco.data", "./darknet/cfg/tiny-yolo.cfg", "tiny-yolo.weights");
console.dir(yolo.detect());