
# node-yolo
This Node.js C++ Addon came out from a computer engineering project, [VAPi](https://github.com/freakstatic/vapi-server).
It allow you to use a state-of-the-art, real-time object detection system called [Yolo](https://pjreddie.com/darknet/yolo/).

### Pre-requirements
* C/C++ Compiler (tested with gcc & g++)
* Nvidia graphic card (Only if you want to use GPU acceleration):
	* [CUDA](https://developer.nvidia.com/cuda-zone)
	* [CuDNN](https://developer.nvidia.com/cudnn)
* [Node.js](https://nodejs.org/en/) (tested on node.js>= 8)
* [node-gyp](https://www.npmjs.com/package/node-gyp)
* [OpenCV](https://opencv.org)

**Note**: The previous version of the module has the [ImageMagick](https://www.imagemagick.org) as a dependency, but with OpenCV we can archive the desired goal. And by this we remove one dependency of the project.

## Installation
```sh
npm i @vapi/node-yolo --save
```

## How To Use

```javascript
const Yolo = require('@vapi/node-yolo');
const detector = new Yolo("darknet-configs", "cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights");
try{
	detector.detectImage(path)
         .then(detections => {
            // here you receive the detections
         })
         .catch(error => {
           // here you can handle the errors. Ex: Out of memory
        });
}
catch(error){
    console.log('Catch: ' + error);
}
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

**Note**: Our suggestion for better performance is to use [coco.data](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.data), [coco.names](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.names), [yolov3-spp.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3-spp.cfg) and [yolov3-spp.weights](https://pjreddie.com/media/files/yolov3-spp.weights).

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
