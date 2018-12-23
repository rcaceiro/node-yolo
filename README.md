# node-yolo
This Node.js C++ addon allow you to use a state-of-the-art, real-time object detection system called [Yolo](https://pjreddie.com/darknet/yolo/).
<br>This addon came out from a computer engineering final project, [VAPi](https://github.com/freakstatic/vapi-server), guided by [Patrício Domingues](https://scholar.google.com/citations?user=LPwSQ2EAAAAJ&hl=en) at [Institute Polytechnic of Leiria](https://www.ipleiria.pt/).
<br>The version 1.x.x was developed by [Rúben Caceiro](https://github.com/rcaceiro) and [Ricardo Maltez](https://github.com/freakstatic) during the final project.
<br>The version 2.x.x was sponsored by [Instituto de Telecomunicações](https://www.it.pt) developed by [Rúben Caceiro](https://github.com/rcaceiro) and guided by [Patrício Domingues](https://scholar.google.com/citations?user=LPwSQ2EAAAAJ&hl=en). 

### Pre-requirements
* C/C++ Compiler (tested with gcc & g++)
* nVidia graphic card (Only if you want to use GPU acceleration):
	* [CUDA](https://developer.nvidia.com/cuda-zone)
	* [CuDNN](https://developer.nvidia.com/cudnn)
* [Node.js](https://nodejs.org/en/) (tested on node.js>= 8)
* [node-gyp](https://www.npmjs.com/package/node-gyp)
* [OpenCV](https://opencv.org)

**Note 1**: Before any update please see the [changelog](https://github.com/rcaceiro/node-yolo/blob/master/CHANGELOG.md).<br>
**Note 2**: The versions prior 2.0.0 of the module has the [ImageMagick](https://www.imagemagick.org) as a dependency, but with OpenCV we can archive the desired goal. And by this we remove one dependency of the project.
### Recommended* hardware requirements
* Quad-core processor**
* 10 GB to run node-yolo
* At least 4GB of GPU memory***, if you want use GPU acceleration
### Minimum* hardware requirements
* Dual-core processor**
* 8 GB to run node-yolo
* At least 4GB of GPU memory***, if you want use GPU acceleration
## Installation
```sh
npm i @vapi/node-yolo --save
```

## How To Use

```javascript
const Yolo = require('@vapi/node-yolo');
const detector = new Yolo("darknet_configs", "cfg/coco.data", "cfg/yolov3.cfg", "yolov3.weights");
detector.detectImage(path,threshold,frames_to_process)
        .then(detections =>
        {
         // here you receive the detections
        })
        .catch(error =>
        {
         // here you can handle the errors. Ex: Out of memory
        });
	
detector.detectVideo(path,threshold,frames_to_process)
        .then(detections =>
        {
         // here you receive the detections
        })
        .catch(error =>
        {
         // here you can handle the errors. Ex: Out of memory
        });
```
**path**, required, represents the path to the file

**threshold**, not required, default behaviour is 0.5, represents the threshold to yolo filter the detections

**frames_to_process**, not required, default behaviour is 1/1, represents the number of frames that developer wants to process, this means 1/3 process 1 frame for each 3.

**darknet_configs** is a folder where you should put the Yolo [weights](https://pjreddie.com/darknet/yolo/), [cfg](https://github.com/pjreddie/darknet/tree/master/cfg) and [data files](https://github.com/pjreddie/darknet/tree/master/data).
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

**Note 1**: Our suggestion for better accuracy use [coco.data](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.data), [coco.names](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.names), [yolov3-spp.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3-spp.cfg) and [yolov3-spp.weights](https://pjreddie.com/media/files/yolov3-spp.weights).
<br>**Note 2**: Our suggestion for faster detection use [coco.data](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.data), [coco.names](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.names), [yolov3-tiny.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3-tiny.cfg) and [yolov3-tiny.weights](https://pjreddie.com/media/files/yolov3-tiny.weights).
<br>**Note 3**: Our suggestion for best of two worlds use [coco.data](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.data), [coco.names](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/coco.names), [yolov3.cfg](https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/yolov3.cfg) and [yolov3.weights](https://pjreddie.com/media/files/yolov3.weights).

#### video detection object
| **Field** | **Type** | **Description**
|:----------|:---------|:-----------------------------------------------------
| `frame` | `long/int64` | number of the frame
| `millisecond` | `double` | the millisecond that frame appear on video
| `timeSpentForClassification` | `double` | time used to classifies one frame
| `detections` | `array` | array of `detection` object, containing all detections

#### image detection object
| **Field** | **Type** | **Description**
|:----------|:---------|:-----------------------------------------------------
| `timeSpentForClassification` | `double` | time used to classifies one image
| `detections` | `array` | array of `detection` object, containing all detections

#### detection object
| **Field** | **Type** | **Description**
|:----------|:---------|:-----------------------------------------------------
| `className`   | `string` | name of the class of the object detected
| `probability` | `double` | the probability that this className is correct
| `box`         | `box` | object that contains box info of the object

#### box object
| **Field** | **Type** | **Description**
|:----------|:---------|:-----------------------------------------------------
| `x`       | `double` | x coordinate in pixels of the picture
| `y`       | `double` | y coordinate in pixels of the picture
| `w`       | `double` | width from x point in pixels
| `h`       | `double` | height from y point in pixels

\* To get that metrics we calculate the usage for video with 3 hours at 60fps.
<br>\**If you do not use gpu, may should consider a processor with higher number of cores.
<br>\***The weaker graphics card used was a nVidia GTX960M