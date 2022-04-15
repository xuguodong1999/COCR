import SmilesDrawer from 'smiles-drawer';

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