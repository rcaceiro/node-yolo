let Yolo = require(__dirname + '/build/Debug/nodeyolojs').Yolo;

let obj1 = new Yolo("./darknet", "./cfg/coco.data", "./cfg/yolov3.cfg", "../yolov3.weights");

// obj1.detect("darknet/data/horses.jpg")
//  .then((detections) => {
//   console.log(1);
//  })
//  .catch((error) => {
//   console.error(error);
//  });
//
obj1.detect("darknet/data/dog.jpg")
 .then((detections) => {
  console.log(JSON.stringify(detections));
 })
 .catch((error) => {
  console.error(error);
 });

// ['dog', 'eagle', 'giraffe', 'horses', 'kite', 'person', 'scream'].forEach((img) => {
//  obj1.detect("./darknet/data/" + img + ".jpg")
//   .then((detections) => {
//    //console.log(img + "\n" + JSON.stringify(detections));
//   })
//   .catch((error) => {
//    console.error(error);
//   });
// });