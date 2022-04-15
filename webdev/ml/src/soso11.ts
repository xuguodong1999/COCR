import * as tf from '@tensorflow/tfjs';
import '@tensorflow/tfjs-backend-webgl';

const soso_classes = [
    '@', '-', '=', '#', '@@',
    'C', 'H', 'O', 'N', 'P', 'S',
];
const anchors_soso11 = [
    10, 10, 5, 30, 30, 5,
    30, 30, 15, 90, 90, 15,
    60, 60, 30, 180, 180, 30
];
const masks = {
    "1": [[0, 1, 2]],
    "2": [[3, 4, 5], [0, 1, 2]],
    "3": [[6, 7, 8], [3, 4, 5], [0, 1, 2]],
    "4": [[9, 10, 11], [6, 7, 8], [3, 4, 5], [0, 1, 2]],
    "5": [[12, 13, 14], [9, 10, 11], [6, 7, 8], [3, 4, 5], [0, 1, 2]],
};

class ShapeData {
    startX: number = 0;
    startY: number = 0;
    endX: number = 0;
    endY: number = 0;
    id: number = 0;
    name: string = '';
    neighbors: Map<number, number | null> = new Map();
    isStartConnected: boolean = false;
    isEndConnected: boolean = false;
    holder: number | null;
    holderId: number | null;

    constructor(arr: any, id: number) {
        this.startX = Math.floor(arr[0]);
        this.startY = Math.floor(arr[1]);
        this.endX = Math.floor(arr[2]);
        this.endY = Math.floor(arr[3]);
        this.id = id;
        this.name = soso_classes[id];
        this.holder = null;
        this.holderId = null;
    }

    //添加图元对应的SmilesDrawer中原子、键的引用和id
    addHolder(holder: number, holderId: number) {
        this.holder = holder;
        this.holderId = holderId;
    }

    //添加原子、键的邻居
    addNeighborId(id: number, info = null) {
        this.neighbors.set(id, info);
        if (this.isBond()) {// 对键来说，记录原子id和一个通过正负表达黏着点的数
            if (typeof info === "number" && info > 0) {
                this.isStartConnected = true;
            } else {
                this.isEndConnected = true;
            }
        }
    }

    getStart() {
        return [this.startX, this.startY];
    }

    getEnd() {
        return [this.endX, this.endY];
    }

    getCent() {
        return [(this.endX + this.startX) / 2, (this.endY + this.startY) / 2];
    }

    getCrossLength() {
        return dis(this.getStart(), this.getEnd());
    }

    isBond() {
        return this.id < 5;
    }

    // 根据像素密度修正指向
    check(imgArr3c: number[][][]) {
        let xmin, ymin, xmax, ymax;
        if (this.startX < this.endX) {
            xmin = this.startX;
            xmax = this.endX;
        } else {
            xmin = this.endX;
            xmax = this.startX;
        }
        if (this.startY < this.endY) {
            ymin = this.startY;
            ymax = this.endY;
        } else {
            ymin = this.endY;
            ymax = this.startY;
        }
        let k = 2;
        let deltaX = Math.floor((xmax - xmin) / k),
            deltaY = Math.floor((ymax - ymin) / k);
        let c = [0, 0, 0, 0], s = [
            [xmin, xmin + deltaX, ymin, ymin + deltaY],
            [xmax - deltaX, xmax, ymax - deltaY, ymax],
            [xmin, xmin + deltaX, ymax - deltaY, ymax],
            [xmax - deltaX, xmax, ymin, ymin + deltaY]
        ]
        for (let i = 0; i < 4; i++) {//统计四个角的像素和
            for (let x = s[i][2]; x <= s[i][3] && imgArr3c[x]; x += 1)
                for (let y = s[i][0]; y <= s[i][1] && imgArr3c[x][y]; y += 1)
                    c[i] -= imgArr3c[x][y][0];
        }
        //找到前两大的端点
        let index = 0;
        for (let i = 1; i < 4; i++) {
            if (c[index] < c[i])
                index = i;
        }
        //顺时针0321
        if (index === 1) {
            [this.startX, this.startY, this.endX, this.endY] = [xmin, ymin, xmax, ymax];
        } else if (index === 0) {
            [this.startX, this.startY, this.endX, this.endY] = [xmax, ymax, xmin, ymin];
        } else if (index === 2) {
            [this.startX, this.startY, this.endX, this.endY] = [xmax, ymin, xmin, ymax];
        } else {
            [this.startX, this.startY, this.endX, this.endY] = [xmin, ymax, xmax, ymin];
        }
    }
}

// 计算 [x1,y1],[x2,y2]的距离
function dis(a: Array<number>, b: Array<number>) {
    let dx = a[0] - b[0],
        dy = a[1] - b[1];
    return Math.sqrt(dx * dx + dy * dy);
}

// 获取canvasSrc对应的canvas元素
function getCanvas(canvasSrc: any) {
    if (typeof canvasSrc === 'string') {
        return document.getElementById(canvasSrc);
    } else {
        return canvasSrc;
    }
}

//解包一个yolov3检测头的输出，有两个检测头
function extractBoxesAndScores(
    outputTensor: tf.TensorContainer,// 1xGrid_WxGrid_Hx48
    anchorTensor: tf.Tensor,// 3x2,[[x1,y1],[x2,y2],[x3,y3]]
    width: number, height: number// 还原到的图像大小
) {
    const numAnchors = anchorTensor.shape[0];
    const numClasses = outputTensor.shape[3] / numAnchors - 5;//48/3-5
    const curAnchors = tf.reshape(anchorTensor, [1, 1, numAnchors, 2]);//1x1x3x2
    const [gridH, gridW] = outputTensor.shape.slice(1, 3); // gridH,gridW
    // 构造坐标表
    let _grid = [];
    for (let i = 0; i < gridH; i++) {
        let _g = []
        for (let j = 0; j < gridW; j++) {
            _g.push([[j, i]]);
        }
        _grid.push(_g);
    }
    //grid的维度：[gridW,gridH,1,2],记录每个位置的坐标,用作一个坐标矩阵,坐标取值[0,w/h_32)
    let grid = tf.tensor(_grid).cast(outputTensor.dtype);
    // grid.print();
    // console.log(grid.shape, grid.arraySync());
    const [xy, wh, conf, prob] = tf.split(// [1,h,w,48]->[h,w,3,16]
        outputTensor.reshape([gridH, gridW, numAnchors, numClasses + 5]),
        [2, 2, 1, numClasses], 3
    );
    //解构前：[wxhx3x2]
    const boxCent = tf.div(tf.add(tf.sigmoid(xy), grid), [gridW, gridH]);
    const boxSize = tf.div(tf.mul(tf.exp(wh), curAnchors), [width, height]);
    const boxConf = tf.sigmoid(conf);
    const classProb = tf.sigmoid(prob);
    const leftTops = tf.mul(tf.sub(boxCent, tf.div(boxSize, 2)), [width, height]);
    const rightButtoms = tf.mul(tf.add(boxCent, tf.div(boxSize, 2)), [width, height]);
    const boxes = tf.concat([//解除数组结构，在坐标维拼接
        ...tf.split(leftTops, 2, 3),//两两拆分，以坐标[x1,y1]为最小单位，3是坐标所在的维度
        ...tf.split(rightButtoms, 2, 3)//若干[x2,y2]
    ], 3).reshape([-1, 4]);//[...,[x1,y1,x2,y2]]
    //boxes.print();
    let boxScores = tf.mul(boxConf, classProb);
    boxScores = tf.reshape(boxScores, [-1, numClasses]);
    return [boxes, boxScores];
}

// 一次完整的yolov3输出矩阵后处理流程
async function postprocess(
    outputTensorArray: tf.TensorContainerArray,
    width: number, height: number,
    anchors: Array<number>,//18x1
    anchorMasks: Array<Array<number>>,//3组
    confThresh = 0.5,
    iouThresh = 0.3,
    maxBoxNum = 2000
) {
    const [boxes, boxMaxScores, boxMaxIndices] = tf.tidy(() => {
        const anchorsTensor = tf.tensor1d(anchors).reshape([-1, 2]);// 维度9x2
        let boxes:tf.Tensor , boxScores :tf.Tensor;
        for (let i = 0; i < outputTensorArray.length; i++) {
            // 取出索引位置的值形成新张量
            const curAnchors = anchorsTensor.gather(tf.tensor1d(anchorMasks[i], 'int32'));
            const [_boxes, _boxScores] = extractBoxesAndScores(
                outputTensorArray[i],
                curAnchors, width, height
            );
            boxes = tf.concat([boxes, _boxes]);
            boxScores = tf.concat([boxScores, _boxScores]);
            outputTensorArray[i].dispose();
        }
        return [boxes, tf.max(boxScores, -1), tf.argMax(boxScores, -1)];
    });
    const result = await tf.image.nonMaxSuppressionWithScoreAsync(
        boxes, boxMaxScores, maxBoxNum, iouThresh, confThresh
    );
    boxMaxScores.dispose();
    let nmsIndex = result["selectedIndices"], ret = [];
    if (nmsIndex.length === 0) {
        boxes.dispose();
        boxMaxIndices.dispose();
        return ret;
    }
    return tf.tidy(() => {
        const classBoxes = tf.gather(boxes, nmsIndex);
        const classIndices = tf.gather(boxMaxIndices, nmsIndex);
        const cb = classBoxes.arraySync(), ci = classIndices.arraySync();
        let l = cb.length;
        for (let i = 0; i < l; i++) {
            ret.push(new ShapeData(cb[i], ci[i]));
        }
        boxes.dispose();
        boxMaxIndices.dispose();
        return ret;
    });
}


export class SOSO11 {
    // 从画布到零散的图元
    static async predict(canvasSrc: string, modelUrl = '/static/js/model.json') {
        // 获取原图
        let canvas = getCanvas(canvasSrc);
        let image = new Image(), canvasInput: HTMLCanvasElement;
        image.src = canvas.toDataURL("image/jpeg", 0.2);
        image.onload = function () {
            const w = canvas.width - canvas.width % 32,
                h = canvas.height - canvas.height % 32;
            // 重绘图像到目标尺寸
            canvasInput = document.createElement('canvas');
            [canvasInput.width, canvasInput.height] = [w, h];
            let ctx = canvasInput.getContext('2d');
            if (ctx)
                ctx.drawImage(image, 0, 0, w, h);
        }
        // 获取结果矩阵和像素图
        const model = await tf.loadLayersModel(modelUrl);
        let imgArr3c;
        const outputs = tf.tidy(() => {
            // 这里认定模型加载比输入图片加载速度慢，image.onload触发的回调在下面的代码执行前完成
            const imageTensor = tf.browser.fromPixels(canvasInput, 3).expandDims(0).toFloat().div(tf.scalar(255));
            imgArr3c = imageTensor.arraySync()[0];
            return model.predict(imageTensor);
        });
        // 转换结果矩阵到图元列表
        let shapes = await postprocess(
            outputs,
            canvasInput.width, canvasInput.height,
            anchors_soso11,
            masks["3"]
        );
        model.dispose();
        // 结合像素图修正图元列表中的线段方向
        let len = shapes.length;
        for (let i = 0; i < len; i++) {
            if (shapes[i].isBond())
                shapes[i].check(imgArr3c);
        }
        return shapes;
    }
}
