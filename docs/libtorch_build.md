# build libtorch

## build cpp docs as pdf

```shell
conda activate nn # need a python3 installation
cd ~/git/pytorch/docs/cpp
pip install -r requirements.txt
make clean && make -j $(nproc) latexpdf
```

## ubuntu host build for gcc

```shell
sudo apt-get install python3-typing-extensions python3-pretty-yaml libmkldnn-dev liblapack-dev
git clone --recursive https://github.com/pytorch/pytorch
git submodule sync
git submodule update --init --recursive

cmake -G "Ninja" ~/git/pytorch -DCMAKE_INSTALL_PREFIX=~/shared/libtorch -DOpenCV_DIR=~/shared/opencv4/lib/cmake/opencv4 -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DUSE_CUDA=ON -DUSE_MKLDNN=ON -DUSE_NCCL=OFF -DUSE_NUMPY=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON -DCMAKE_INSTALL_DOCDIR=~/shared/libtorch/doc
```

### windows host build for msvc

```shell
set SHARED_LIB_DIR=C:/Library/Shared
# debug
cmake D:/git/pytorch -G "Ninja" -DCMAKE_INSTALL_PREFIX=%SHARED_LIB_DIR%/libtorch -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DOpenCV_DIR=%SHARED_LIB_DIR%/opencv4/x64/vc16/lib -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DUSE_CUDA=ON -DUSE_MKLDNN=OFF -DUSE_NCCL=OFF -DUSE_NUMPY=OFF -DUSE_MKLDNN=ON -DCMAKE_BUILD_TYPE=Debug
# release
cmake D:/git/pytorch -G "Ninja" -DCMAKE_INSTALL_PREFIX=%SHARED_LIB_DIR%/libtorch -DBUILD_PYTHON=OFF -DUSE_OPENCV=ON -DOpenCV_DIR=%SHARED_LIB_DIR%/opencv4/x64/vc16/lib -DUSE_DISTRIBUTED=OFF -DUSE_TENSORPIPE=OFF -DBUILD_TEST=ON -DUSE_CUDA=ON -DUSE_MKLDNN=OFF -DUSE_NCCL=OFF -DUSE_MKLDNN=ON  -DUSE_NUMPY=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON -DCMAKE_INSTALL_DOCDIR=%SHARED_LIB_DIR%/libtorch/doc
```
