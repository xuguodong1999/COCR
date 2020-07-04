# COCR 开发分支

## 计划和思路

* [finished] 构建过程全面脚本化，摆脱手动下载编译链接各个库的过程
* [finished] 支持 linux 和 win10 PC端 OpenBabel>=3.0.0、OpenCV>=4.3的自动构建、运行、插件加载
* [doing...] 用 c++ 重写 ChemInk 里 c with class 的代码，重构 COCR 主分支中数据集生成代码
* [planning] 客户端开发彻底 Qt 化，充分利用框架提供的序列化、线程管理、网络通信机制，从长计议开发桌面应用
* [planning] 客户端的需求列表：
* [planning] OpenBabel 的图形界面
* [planning] 基于控件拖动的结构式编辑器
* [planning] 基于手势交互的结构式编辑器
* [planning] 基于目标检测的结构式编辑器
* [planning] 抄写并改进 OpenBabel 的设计，去 OpenBabel 化
* [planning] 先这样吧

## 构建

* 假设下面的命令行都能找到 git 和 cmake 命令

### linux

```shell
# 打开 shell
./script/init.sh
./script/build.sh
```

### windows

```bat
REM 打开 MSVC x64 命令行工具，进入 COCR 根目录
.\script\init.bat
.\script\build.bat
```

## Qt引入

* 修改 CMakeList.txt 中 Qt 配置
```txt
if (UNIX)       # FIXME: linux下修改到Qt库的cmake文件夹位置
    set(Qt5_DIR /home/xgd/Qt/5.15.0/gcc_64/lib/cmake/Qt5)
elseif (MSVC)   # FIXME: windows下修改到Qt库的cmake文件夹位置
    set(Qt5_DIR C:/Qt/5.12.6/msvc2019_64/lib/cmake/Qt5)
    # set(Qt5_DIR lib/Qt/5.15.0/msvc2017_64/lib/cmake/Qt5)
endif ()
```
* windows 下要求对 Qt 进行静态编译(实在搞不定win下OpenBabel动态编译的插件加载问题，索性在win下统统静态编译了)，好消息是，到了 Qt 5.15 这一版，编译不需要什么特别配置了，用 vs2019 默认的 MSVC x64 命令行工具就行，参考命令：
* 打开 git bash
```shell
curl -O https://download.qt.io/archive/qt/5.15/5.15.0/single/qt-everywhere-src-5.15.0.tar.xz
tar -xvf qt-everywhere-src-5.15.0.tar.xz -C "somewhere/ with/ at/ least/ 3/ GB/"
```

* 打开 MSVC x64 命令行工具，进入一个空目录，预留至少20GB

```bat
..\path to\qt-everywhere-src-5.15.0\configure.bat -static -prefix "path/ to/ install/ dir/ rem/ to/ change/ Qt5_DIR/ in/ CMakeLists.txt/" -confirm-license -opensource -debug-and-release -platform win32-msvc -nomake examples -nomake tests -mp
nmake && nmake install
```