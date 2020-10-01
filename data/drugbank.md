# 从 drugbank 拿到的 SMILES 串、ChemScript 生成的坐标

## 内部资料，不得传播！！！务必！！！

* drugbank.json 包含经过 ChemScript 清洗原始 SMILES 串和所有坐标、电荷、化学键类型信息
* drugbank_h.json 中的所有 SMILES 串以及坐标都补全了氢原子
* json 文件结构：一个对象数组，每个对象代表一个结构式，有 "smiles","atoms","bonds"三个字段，字段内容如下
* 字段说明：
  1. "smiles":字符串
  2. "atoms":对象数组，每个对象表示一个原子，包含4个字段
       1. "id":原子id，从1开始顺序增加，整形数
       2. "element":元素符号，字符串
       3. "x"、"y":坐标，浮点数
       4. "charge":带符号整型数，标识原子电荷
  3. "bonds":对象数组，每个对象表示一个化学键，包含4个字段
       1. "from": 整型数，原子id，化学键起点
       2. "to": 整型数，原子id，化学键终点
       3. "type": 字符串，化学键类型，包括"Single","Double","Triple"
       4. "stereo": 字符串，异构类型，包括"None","Up","Down"

* 一个例子，包括两个结构式
```json
[
    {
        "smiles": "CC(C(O)=O)=O",
        "atoms": [
            {
                "id": 1,
                "element": "C",
                "x": 8.660,
                "y": 5.000,
                "charge": 0
            },
            {
                "id": 2,
                "element": "C",
                "x": 17.321,
                "y": 0.000,
                "charge": 0
            },
            {
                "id": 3,
                "element": "C",
                "x": 25.981,
                "y": 5.000,
                "charge": 0
            },
            {
                "id": 4,
                "element": "O",
                "x": 34.641,
                "y": 0.000,
                "charge": 0
            },
            {
                "id": 5,
                "element": "O",
                "x": 25.981,
                "y": 15.000,
                "charge": 0
            },
            {
                "id": 6,
                "element": "O",
                "x": 17.321,
                "y": -10.000,
                "charge": 0
            }
        ],
        "bonds": [
            {
                "from": 1,
                "to": 2,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 2,
                "to": 3,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 3,
                "to": 4,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 3,
                "to": 5,
                "type": "Double",
                "stereo": "None"
            },
            {
                "from": 2,
                "to": 6,
                "type": "Double",
                "stereo": "None"
            }
        ]
    },
    {
        "smiles": "CC([O-])=O.CC([O-])=O.[Ca+2]",
        "atoms": [
            {
                "id": 1,
                "element": "C",
                "x": -8.660,
                "y": -10.000,
                "charge": 0
            },
            {
                "id": 2,
                "element": "C",
                "x": -0.000,
                "y": -5.000,
                "charge": 0
            },
            {
                "id": 3,
                "element": "O",
                "x": 8.660,
                "y": -10.000,
                "charge": -1
            },
            {
                "id": 4,
                "element": "O",
                "x": 0.000,
                "y": 5.000,
                "charge": 0
            },
            {
                "id": 5,
                "element": "C",
                "x": 18.660,
                "y": -10.000,
                "charge": 0
            },
            {
                "id": 6,
                "element": "C",
                "x": 27.321,
                "y": -5.000,
                "charge": 0
            },
            {
                "id": 7,
                "element": "O",
                "x": 35.981,
                "y": -10.000,
                "charge": -1
            },
            {
                "id": 8,
                "element": "O",
                "x": 27.321,
                "y": 5.000,
                "charge": 0
            },
            {
                "id": 9,
                "element": "Ca",
                "x": 45.981,
                "y": -2.500,
                "charge": 2
            }
        ],
        "bonds": [
            {
                "from": 1,
                "to": 2,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 2,
                "to": 3,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 2,
                "to": 4,
                "type": "Double",
                "stereo": "None"
            },
            {
                "from": 5,
                "to": 6,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 6,
                "to": 7,
                "type": "Single",
                "stereo": "None"
            },
            {
                "from": 6,
                "to": 8,
                "type": "Double",
                "stereo": "None"
            }
        ]
    }
]
```