import * as tf from '@tensorflow/tfjs';
import '@tensorflow/tfjs-backend-webgl';
import SmilesDrawer from './smiles-drawer';

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

function ShapeData(arr, id) {
    this.startX = Math.floor(arr[0]);
    this.startY = Math.floor(arr[1]);
    this.endX = Math.floor(arr[2]);
    this.endY = Math.floor(arr[3]);
    this.id = id;
    this.name = soso_classes[id];
    this.neighbors = {};
    this.isStartConnected = false;
    this.isEndConnected = false;
    this.holder = null;
    this.holderId = null;
}

// 成员函数
ShapeData.prototype = {
    //添加图元对应的SmilesDrawer中原子、键的引用和id
    addHolder: function (holder, holderId) {
        this.holder = holder;
        this.holderId = holderId;
    },
    //添加原子、键的邻居
    addNeighborId: function (id, info = null) {
        this.neighbors[id] = info;
        if (this.isBond()) {// 对键来说，记录原子id和一个通过正负表达黏着点的数
            if (typeof info === "number") {
                if (1 / info > 0) {//利用1/(+0)>0的性质
                    this.isStartConnected = true;
                } else {
                    this.isEndConnected = true;
                }
            }
        }
    },
    getStart: function () {
        return [this.startX, this.startY];
    },
    getEnd: function () {
        return [this.endX, this.endY];
    },
    getCent: function () {
        return [(this.endX + this.startX) / 2, (this.endY + this.startY) / 2];
    },
    getCrossLength: function () {
        return dis(this.getStart(), this.getEnd());
    },
    isBond: function () {
        return this.id < 5;
    },
    // 根据像素密度修正指向
    check: function (imgArr3c) {
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
function dis(a, b) {
    let dx = a[0] - b[0],
        dy = a[1] - b[1];
    return Math.sqrt(dx * dx + dy * dy);
}

// 获取canvasSrc对应的canvas元素
function getCanvas(canvasSrc) {
    if (typeof canvasSrc === 'string' || canvasSrc instanceof String) {
        return document.getElementById(canvasSrc);
    } else {
        return canvasSrc;
    }
}

//解包一个yolov3检测头的输出，有两个检测头
function extractBoxesAndScores(
    outputTensor,// 1xGrid_WxGrid_Hx48
    anchorTensor,// 3x2,[[x1,y1],[x2,y2],[x3,y3]]
    width, height// 还原到的图像大小
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
    outputTensorArray,
    width, height,
    anchors,//18x1
    anchorMasks,//3组
    confThresh = 0.5,
    iouThresh = 0.3,
    maxBoxNum = 2000
) {
    const [boxes, boxMaxScores, boxMaxIndices] = tf.tidy(() => {
        const anchorsTensor = tf.tensor1d(anchors).reshape([-1, 2]);// 维度9x2
        let boxes = [], boxScores = [];
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

// 结构综合，计算图元的连接关系
function handleMolBoxes(canvasDst, shapes) {
    let canvas = getCanvas(canvasDst);
    let acents = [], //shapeId,中心坐标
        bsides = [], //(+-)shapeId,端点坐标
        avgSize = 0;
    for (let i = 0; i < shapes.length; i++) {
        if (!shapes[i].isBond()) {
            acents.push([i, shapes[i].getCent()]);
        } else {
            bsides.push([i, shapes[i].getStart()]);
            bsides.push([-i, shapes[i].getEnd()]);//约定，键的起点标记为负id
        }
        avgSize += shapes[i].getCrossLength();
    }
    avgSize /= (shapes.length + 1);
    let K_2 = avgSize / 2;
    // 填写键图元的原子id
    for (let i = 0; i < acents.length; i++) {
        let acent = acents[i];
        let atom = shapes[acent[0]];
        let maxCon = 4;//表达最大键合数
        if (atom.name === 'O' || atom.name === 'S')
            maxCon = 2;
        else if (atom.name === "N" || atom.name === "P")
            maxCon = 3;

        function sortByDistanceBetweenAtomCentAndBondSide(structA, structB) {
            return dis(acent[1], structA[1]) - dis(acent[1], structB[1]);
        }

        //根据一个原子到所有键端的距离排序键端
        bsides.sort(sortByDistanceBetweenAtomCentAndBondSide);
        let _bids = {};//表达临近结合规则
        for (let j = 0; j < bsides.length; j++) {
            if (dis(acent[1], bsides[j][1]) > K_2)
                break;
            if (_bids[Math.abs(bsides[j][0])] === undefined) {
                _bids[Math.abs(bsides[j][0])] = bsides[j][0];
                shapes[Math.abs(bsides[j][0])].addNeighborId(acent[0], bsides[j][0]);
            }
            if (_bids.length >= maxCon)
                break;
        }
        if (_bids.length === 0 && bsides.length > 0) {//保证至少有一个键端关联到这个原子
            _bids[Math.abs(bsides[0][0])] = bsides[0][0];
            shapes[Math.abs(bsides[0][0])].addNeighborId(acent[0], bsides[0][0]);
        }
        //console.log('atom_', acent[0], _bids);
    }
    //准备用于伪聚类的数据结构
    bsides = [];// shapeId,Position,isStart,groupId
    for (let i = 0; i < shapes.length; i++) {
        let bond = shapes[i];
        if (!bond.isBond()) continue;
        if (!bond.isStartConnected) {
            bsides.push([i, bond.getStart(), true, null]);
        }
        if (!bond.isEndConnected) {
            bsides.push([i, bond.getEnd(), false, null]);
        }
    }
    //本质是对键端坐标进行聚类，简化为考虑距离阈值K_2
    let groups = [];//groupCenter,bidList
    for (let i = 0; i < bsides.length; i++) {
        let b = bsides[i];
        if (b[3] != null) continue;
        let succ = false;
        for (let j = 0; j < groups.length; j++) {
            let g = groups[j][0];//聚类中心坐标
            if (dis(b[1], g) < K_2) {
                b[3] = j;
                groups[j][1].push([b[0], b[2]]);
                g = [(g[0] + b[1][0]) / 2, (g[1] + b[1][1]) / 2];//调整聚类中心
                succ = true;
                break;
            }
        }
        if (!succ) {
            b[3] = groups.length;
            groups.push([
                b[1],
                [[b[0], b[2]]]
            ]);
        }
    }
    for (let i = 0; i < groups.length; i++) {
        let g = groups[i];
        let aid = shapes.length;
        shapes.push(new ShapeData([g[0][0], g[0][1], g[0][0], g[0][1]], 5));//添加一个不关心其位置的碳原子
        for (let j = 0; j < g[1].length; j++) {
            let id = g[1][j][0];//键id
            let isStart = g[1][j][1];
            if (isStart)
                shapes[id].addNeighborId(aid, +1);
            else
                shapes[id].addNeighborId(aid, -1);
        }
    }
    for (let i = 0; i < shapes.length; i++) {
        let s = shapes[i];
        if (!s.isBond()) continue;
        let aid = [];
        for (let a in s.neighbors) {
            aid.push(a);
        }
        shapes[aid[0]].addNeighborId(aid[1]);
        shapes[aid[1]].addNeighborId(aid[0]);
    }
    // console.log(acents);
    // console.log(groups);
    console.log([shapes]);
    // 构造连接关系
    let g = new SmilesDrawer.Graph(null);
    //遍历原子，让shape持有原子图元的引用
    for (let i = 0; i < shapes.length; i++) {
        let s = shapes[i];
        if (s.isBond()) continue;
        let atom = new SmilesDrawer.Atom(s.name);
        let vertex = new SmilesDrawer.Vertex(atom);
        //vertex.setPosition(s.getCent()[0], s.getCent()[1]);
        //vertex.positioned = true;
        let aid = g.addVertex(vertex);
        s.addHolder(vertex, aid);
    }
    //遍历原子，关联原子图元的邻居
    // for (let i = 0; i < shapes.length; i++) {
    //     let s = shapes[i];
    //     if (s.isBond()) continue;
    //     for (let a in s.neighbors) {
    //         if (a % 2 == 0)
    //             s.holder.addChild(shapes[a].holderId);
    //         else
    //             s.holder.setParentVertexId(shapes[a].holderId);
    //     }
    // }
    //遍历键，生成键图元
    for (let i = 0; i < shapes.length; i++) {
        let s = shapes[i];
        if (!s.isBond()) continue;
        let aid = [];
        for (let a in s.neighbors) {
            aid.push(a);
        }
        let bt = '-', iso = '',//键类型，楔形键类型
            weight = 1;
        if (s.name === '@') {
            iso = 'down';
        } else if (s.name === '@@') {
            iso = 'up';
        } else if (s.name === '#') {
            bt = s.name;
            weight = 3;
        } else if (s.name === '=') {
            bt = s.name;
            weight = 2;
        }
        let e = new SmilesDrawer.Edge(
            shapes[aid[0]].holderId, shapes[aid[1]].holderId,
            weight, bt, iso
        );
        g.addEdge(e);
        shapes[aid[0]].holder.addChild(shapes[aid[1]].holderId);
        shapes[aid[1]].holder.setParentVertexId(shapes[aid[0]].holderId);
    }
    console.log(g);
    let drawer = new SmilesDrawer.Drawer({
        width: canvas.width,
        height: canvas.height,
        debug: false,
    });
    try {
        drawer.drawGraph(g, canvas, 'bw');
    } catch (e) {
        console.log(e);
    }
}

// 测试SmilesDrawer的功能
function testDrawer(canvasSrc) {
    console.log('testDrawer...');
    let canvas;
    if (typeof canvasSrc === 'string' || canvasSrc instanceof String) {
        canvas = document.getElementById(canvasSrc);
    } else {
        canvas = canvasSrc;
    }
    let g = new SmilesDrawer.Graph(null);
    let atom = new SmilesDrawer.Atom('C');
    let v1 = new SmilesDrawer.Vertex(atom);
    atom = new SmilesDrawer.Atom('C');
    let v2 = new SmilesDrawer.Vertex(atom);
    atom = new SmilesDrawer.Atom('N');
    let v3 = new SmilesDrawer.Vertex(atom);
    let a1 = g.addVertex(v1);
    let a2 = g.addVertex(v2);
    let a3 = g.addVertex(v3);
    v1.setParentVertexId(a2);
    v1.setParentVertexId(a3);
    v3.setParentVertexId(a1);
    v2.setParentVertexId(a1);
    let e = new SmilesDrawer.Edge(a2, a1, 1, '-', 'up');
    g.addEdge(e);
    e = new SmilesDrawer.Edge(a3, a1, 1, '-', 'down');
    g.addEdge(e);
    let drawer = new SmilesDrawer.Drawer({
        width: canvas.width,
        height: canvas.height,
        debug: false,
    });
    try {
        drawer.drawGraph(g, canvasSrc);
    } catch (e) {
        console.log(e);
    }
    console.log(g);
}


export default class COCR {
    // 从画布到零散的图元
    static async predict(canvasSrc, modelUrl = '/static/js/model.json') {
        // 获取原图
        let canvas = getCanvas(canvasSrc);
        let image = new Image(), canvasInput;
        image.src = canvas.toDataURL("image/jpeg", 0.2);
        image.onload = function () {
            const w = canvas.width - canvas.width % 32,
                h = canvas.height - canvas.height % 32;
            // 重绘图像到目标尺寸
            canvasInput = document.createElement('canvas');
            [canvasInput.width, canvasInput.height] = [w, h];
            let ctx = canvasInput.getContext('2d');
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
