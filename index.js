//module.exports = require(__dirname + '/build/Release/nodeyolo').Yolo;

let yolo_addon = require(__dirname + '/build/Debug/nodeyolo').Yolo;

let obj = new yolo_addon("./darknet", "./cfg/coco.data", "./cfg/yolov3-spp.cfg", "../weights/yolov3-spp.weights");

// obj.detectImage("./darknet/data/kite.jpg")
//     .then((detections) => {
//         console.log(JSON.stringify(detections));
//     })
//     .catch((error) => {
//         console.error(error);
//     });

obj.detectVideo("/home/rcaceiro/Documents/IPLeiria/research_grant/node-yolo/data/drop.avi")
    .then((detections) => {
        console.log(JSON.stringify(detections));
    })
    .catch((error) => {
        console.error(error);
    });