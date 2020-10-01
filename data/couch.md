# SCUT-IRAC-COUCH 数据集：数字、字母、特殊字符部分

## 内部资料，不得传播！！！务必！！！

## 转制成简明格式，读取说明如下：
```txt
迭代以下过程直到文件末尾：
loop until the end of couch.dat:
    a. 读取4个整型数，分别代表【标签索引】、【宽度】、【长度】、【笔画数目】
    read 4 int numbers, namely label_index, width, height and number of strokes
    b. 迭代【笔画数目】次，每次迭代进行以下过程：
    loop number of strokes times
        a. 读取1个整型数，代表这个笔画的【点数】
        read one int number, representing number of points in this stroke
        b. 迭代【点数】次，每次迭代进行以下过程：
        loop number of points times
            a. 读取2个浮点数，代表这个点的【横坐标】、【纵坐标】
            read two float numbers, namely x,y for one point

备注：整型数是c++下的int类型，浮点数是c++下的float类型，其它编程语言可用同样长度的字段获取数据
ps: int and float are meant for those in c++ in a x64 machine.
```

* 【标签索引】和明文【标签】的转换见两个纯文本文件，注意严格区分字符编码
* 所有明文标签都**不是**ASCII字符