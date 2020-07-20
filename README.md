# COCR 开发分支

## 进度

* 【完结】 构建过程全面脚本化，摆脱手动下载编译链接各个库的过程
* 【完结】 支持 linux 和 win10 PC端 OpenBabel>=3.0.0、OpenCV>=4.3的自动构建、运行、插件加载
* 【正在做……】 用 c++ 重写 ChemInk 里 c with class 的代码，重构 COCR 主分支中数据集生成代码
* 【正在做……】 跑通OpenCV和OpenBabel的安卓构建，尝试编写qml
* 【计划中……】 客户端开发彻底 Qt 化，充分利用框架提供的序列化、线程管理、网络通信机制，从长计议开发桌面应用
* 【计划中……】 客户端的需求列表：
* 【计划中……】 基于控件拖动的结构式编辑器
* 【计划中……】 基于手势交互的结构式编辑器
* 【计划中……】 基于目标检测的结构式编辑器
* 【计划中……】 OpenBabel 的图形界面，抄写并改进 OpenBabel 的设计，去 OpenBabel
* 【计划中……】 Gromacs 的图形界面

## 代码结构

1. 3rdparty 目录：通过submodule引用的其它项目
2. include 目录：当前开发版本的头文件
3. src 目录：当前开发版本的源文件和CMake配置文件
4. test 目录：当前开发分支的gtest测试源文件
5. res 目录：资源文件和qrc资源引用文件
6. script 目录：submodule初始化脚本和第三方库编译脚本
7. version 目录：存放历史版本（因为码龄3年的萌新的代码能力在快速变化，看几个月前的代码像屎山……）
8. android 目录：qmake构建配置，考虑到qt在快速更新对安卓的支持（就是现在的支持不够完善），始终使用qt-creator作为COCR安卓开发的IDE
9. . 目录：COCR 根目录存放CMake配置文件，引用来自version下各个版本子工程、src下的开发子工程和test测试子工程的CMake配置文件

## 第三方库的构建

* Linux
```shell
# 拉取项目代码
$(git, CMake) git clone https://github.com/Xuguodong1999/COCR.git -b dev && cd COCR
# 从github拉取第三方库
$(git, CMake) git submodule update --init --recursive
# checkout指定版本，按照设定的选项编译
$(git, CMake) ./script/build.sh
```

* Windows
```bat
REM 拉取项目代码
$(git, CMake) git clone https://github.com/Xuguodong1999/COCR.git -b dev && cd COCR
REM 从github拉取第三方库
$(MSVC x64, git, CMake) git submodule update --init --recursive
REM checkout指定版本，按照设定的选项编译
$(MSVC x64, git, CMake) .\script\build.bat
```

* Android
```shell
7z x android/openbabel.7z -oandroid
wget https://github.com/opencv/opencv/releases/download/4.3.0/opencv-4.3.0-android-sdk.zip
unzip opencv-4.3.0-android-sdk.zip -d android/
mv android/OpenCV-android-sdk android/opencv
# OpenBabel for Android 的编译命令
# export NDK_ROOT=/home/xgd/Android/Sdk/ndk/21.1.6352462
# mkdir build-openbabel-android && cd build-openbabel-android
# cmake ../3rdparty/openbabel -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../android/openbabel -DBUILD_TESTING=OFF -DRAPIDJSON_INCLUDE_DIRS=../lib/rapidjson/include/rapidjson -DBUILD_GUI=OFF -DWITH_INCHI=OFF -DWITH_STATIC_INCHI=OFF -DOPENBABEL_USE_SYSTEM_INCHI=OFF -DWITH_MAEPARSER=OFF -DANDROID_ABI="armeabi-v7a with NEON" -DCMAKE_TOOLCHAIN_FILE="/home/xgd/Android/Sdk/ndk/21.1.6352462/build/cmake/android.toolchain.cmake" -DANDROID_NATIVE_API_LEVEL=21 -DANDROID_NDK="${NDK_ROOT}" -DBUILD_SHARED=OFF 
```

## Qt引入

* 修改 CMakeList.txt 中 Qt 配置
```txt
if (UNIX) # FIXME: linux下修改到Qt库的cmake文件夹位置
 set(Qt5_DIR /home/xgd/Qt/5.15.0/gcc_64/lib/cmake/Qt5)
elseif (MSVC) # FIXME: windows下修改到Qt库的cmake文件夹位置
 set(Qt5_DIR C:/Qt/5.12.6/msvc2019_64/lib/cmake/Qt5)
 # set(Qt5_DIR lib/Qt/5.15.0/msvc2017_64/lib/cmake/Qt5)
endif ()
```

* ~~windows 下要求对 Qt 进行静态编译(实在搞不定win下OpenBabel动态编译的插件加载问题，索性在win下统统静态编译了)~~
* 实测发现 Qt官方提供的 5.12-5.15 MSVC2017 动态库可以和其它静态库混合链接（可能是去掉了某些指示库类型的符号，自己按默认库类型选项编译的库做不到在windows下混合链接）

## Qt 5.12-5.15 MSVC2017或2019 静态编译的参考过程
* 准备：可以没有ruby和perl，但是必须有python，2.x和3.x均可，必须安装过Visual Studio 2017/2019 的 c++ 组件
* 拉取源码：打开 git bash
```shell
$(gitbash) curl -O https://download.qt.io/archive/qt/5.15/5.15.0/single/qt-everywhere-src-5.15.0.tar.xz
$(gitbash) tar -xvf qt-everywhere-src-5.15.0.tar.xz -C "somewhere/ with/ at/ least/ 3/ GB/"
```

* 修改一个配置
```txt
打开 qtbase\mkspecs\common\msvc-desktop.conf ，替换所有-MD为-MT，应该有3处
```

* 编译源码：打开 MSVC x64 命令行工具，进入一个空目录，预留至少20GB
```bat
$(MSVC x64) ..\path to\qt-everywhere-src-5.15.0\configure.bat -static -prefix "path/ to/ install/ dir/ rem/ to/ change/ Qt5_DIR/ in/ CMakeLists.txt/" -confirm-license -opensource -debug-and-release -platform win32-msvc -nomake examples -nomake tests -mp
$(MSVC x64) nmake && nmake install
```

## 2020年2月，大创中期检查的效果展示

![目标检测示例](./res/assert/screenshot-2020-0217.jpg)
![图元编辑示例](./res/assert/screenshot-2020-0222.gif)