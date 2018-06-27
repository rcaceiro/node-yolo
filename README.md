
# node-yolo
This project came out from a computer engineering project [VAPi](https://github.com/freakstatic/vapi-server).
Our project uses a bleeding edge AI algorithm that classify objects of a certain image called [yolo](https://pjreddie.com/darknet/yolo/).

### Pre-requirements
* C/C++
* CUDA (If you want to use GPU accelaration, only nVidia)
* [nodeJS](https://nodejs.org/en/)>=8
* [node-gyp](https://www.npmjs.com/package/node-gyp)

## Installation
```sh
npm install https://github.com/rcaceiro/node-yolo --save
```

## How To Use
**darknet-configs** is a folder where you should put the weight files and you most create inside of it another called cfg where put the config files.
```javascript
const yolo = require('node-yolo');
const detector = new yolo("darknet-configs", "cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights");
detector.detect(path)
        .then(detections => {
        })
        .catch(error => {
        });
```
#### detections object
| **Field**   | **Description**
|:--------------|:---------------------------------------------------------------
| `className`   | Name of the class of the object detected
| `probability` | The higher probability that this className is correct
| `box`         | obejct that contains box info of the object

#### box object
| **Field**   | **Description**
|:--------------|:---------------------------------------------------------------
| `x`           | x coordinate in pixels of the picture
| `y`           | y coordinate in pixels of the picture
| `w`           | width from x point in pixels
| `h`           | height from y point in pixels
