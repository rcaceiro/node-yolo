//module.exports = require(__dirname + '/build/Release/nodeyolo').Yolo;
let fs = require('fs');

let yolo_addon = undefined;
if (fs.existsSync(__dirname + '/build/Debug/nodeyolo.node')) {
    yolo_addon = require(__dirname + '/build/Debug/nodeyolo').Yolo;
}
else if (fs.existsSync(__dirname + '/build/Release/nodeyolo.node')) {
    yolo_addon = require(__dirname + '/build/Release/nodeyolo').Yolo;
}
else {
    return;
}

let obj = new yolo_addon("./darknet", "./cfg/coco.data", "./cfg/yolov3-spp.cfg", "../weights/yolov3-spp.weights");

let isMac = process.platform === "darwin";

if (isMac) {
    obj.detectImage("./darknet/data/kite.jpg")
        .then((detections) => {
            console.log(JSON.stringify(detections));
        })
        .catch((error) => {
            console.error(error);
        });
}
else {
    if (process.argv[2] !== undefined) {
        obj.detectVideo(process.argv[2])
            .then((detections) => {
                console.log(JSON.stringify(detections));
            })
            .catch((error) => {
                console.error(error);
            });
    }
}