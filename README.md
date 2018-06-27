
# node-yolo
This Node.js C++ Addon came out from a computer engineering project, [VAPi](https://github.com/freakstatic/vapi-server).
It allow you to use a state-of-the-art, real-time object detection system called [Yolo](https://pjreddie.com/darknet/yolo/).

### Pre-requirements
* C/C++ Compiler
* Nvidia graphic card with [CUDA](https://developer.nvidia.com/cuda-downloads) support and required files installed (Only if you want to use GPU acceleration)
* [Node.js](https://nodejs.org/en/) >= 8
* [node-gyp](https://www.npmjs.com/package/node-gyp)

## Installation
```sh
npm i @vapi/node-yolo --save
```

## How To Use

```javascript
const yolo = require('node-yolo');
const detector = new yolo("darknet-configs", "cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights");
detector.detect(path)
    .then(detections => {
        // here you receive the detections
    })
    .catch(error => {
        // here you can handle the errors. Ex: Out of memory
    });
```
**darknet-configs** is a folder where you should put the Yolo [weights](https://pjreddie.com/darknet/yolo/), [cfg](https://github.com/pjreddie/darknet/tree/master/cfg) and [data files](https://github.com/pjreddie/darknet/tree/master/data). 
You need to create two folder, cfg and data and put the files for each one. Like this:<br/>

    .
    ├── darknet-configs         # The folder for the Yolo weight, cfg and data files
    │   ├── cfg                 # cfg folder
    |          |── coco.data
    |          |── yolov3.cfg
    │   ├── data                # data folder
    |   |      |── coco.names
    │   └── yolov3.weights      # YoloV3 weights file
    └── ...




#### detections object
| **Field**   | **Description**
|:--------------|:---------------------------------------------------------------
| `className`   | name of the class of the object detected
| `probability` | the higher probability that this className is correct
| `box`         | object that contains box info of the object

#### box object
| **Field**   | **Description**
|:--------------|:---------------------------------------------------------------
| `x`           | x coordinate in pixels of the picture
| `y`           | y coordinate in pixels of the picture
| `w`           | width from x point in pixels
| `h`           | height from y point in pixels
