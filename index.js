//module.exports = require(__dirname + '/build/Release/nodeyolo').Yolo;

let fs = require('fs');

let yolo_addon = undefined;
if (fs.existsSync(__dirname + '/build/Debug/nodeyolo.node')) {
    yolo_addon = require(__dirname + '/build/Debug/nodeyolo').Yolo;
} else if (fs.existsSync(__dirname + '/build/Release/nodeyolo.node')) {
    yolo_addon = require(__dirname + '/build/Release/nodeyolo').Yolo;
} else {
    return;
}

let obj = new yolo_addon("./darknet", "./cfg/coco.data", "./cfg/yolov3-spp.cfg", "../weights/yolov3-spp.weights");

if (process.argv[2] === undefined) {
    //obj.detectImage("./darknet/data/kite.jpg", 0.55)
    obj.detectImage("/home/rcaceiro/SynologyDrive/IPLeiria/research_grant/node-yolo/data/015-TSmith2-sec-crop-1-300x300.jpg", 0.55)
        .then((detections) => {
            console.log(JSON.stringify(detections));
        })
        .catch((error) => {
            console.error(error);
        });
} else if (process.argv[2] !== undefined) {

    if (!fs.lstatSync(process.argv[2]).isDirectory()) {
        obj.detectVideo(process.argv[2], 0.5, 1 / 2)
            .then((detections) => {
                console.log(JSON.stringify(detections));
            })
            .catch((error) => {
                console.error(error);
            });
    } else {
        fs.readdir(process.argv[2], {
            encoding: 'utf8',
            withFileTypes: true
        }, (err, files) => {
            if (err) {
                console.log(err);
            }
            for (let file of files) {
                console.log(process.argv[2] + file.name);
                obj.detectVideo(process.argv[2] + file.name, 0.5, 1)
                    .then((detections) => {
                        //console.log(file + " " + JSON.stringify(detections));
                        fs.unlinkSync(process.argv[2] + file.name)
                        console.log(file.name + " done!");
                    })
                    .catch((error) => {
                        console.error(error);
                    });
            }
        });
    }
}
