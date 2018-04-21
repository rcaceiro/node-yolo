let yolo_addon = require('/Users/rcaceiro/CloudStation/IPLeiria/3year/Project/NodeYoloJS/build/Debug/nodeyolojs')

// class YoloJS {
//  constructor(working_directory, datacfg, cfgfile, weightfile) {
//   this.yolo = new yolo_addon.Yolo(working_directory, datacfg, cfgfile, weightfile);
//  }
//
//  detect(image_path) {
//   let that=this;
//   return new Promise(function (resolve, reject) {
//    let detections = that.yolo.detect(image_path);
//    if (detections !== undefined) {
//     resolve(detections);
//    }
//    else {
//     reject("Cannot get detections to image " + image_path);
//    }
//   });
//  }
// }

let obj = new yolo_addon.Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov2-tiny.cfg", "../yolov2-tiny.weights");
obj.detect("darknet/data/dog.jpg")
 .then((detections) => {
  console.log(JSON.stringify(detections));
 })
 .catch((error) => {
  console.error(error);
 });

console.log('almost done');

let x = 0;
setInterval(function () {
 console.log(x);
 x++;
}, 1000);

// // let yolo1 = new addons.Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov2-tiny.cfg", "../yolov2-tiny.weights");
//let obj = yolo.detect("darknet/data/dog.jpg");
// console.log(JSON.stringify(obj));
// // let obj1=yolo1.detect("darknet/data/kite.jpg");
// // console.log(JSON.stringify(obj1));