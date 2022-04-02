# android build

## ubuntu host build for android platform

due to qt-community-lts issues, qt-5.15.2 only support single project-level apk build

```shell
# install system glslang
sudo apt-get install glslang-dev

# config
cmake .. -G "Ninja" \
    -DQt5_DIR:PATH=~/shared/Qt/5.15.2/android/lib/cmake/Qt5 \
    -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake \
    -DANDROID_PLATFORM=30 \
    -DANDROID=ON \
    -DANDROID_ABI=armeabi-v7a \
    -DJAVA_HOME=~/shared/jdk1.8.0_30 \
    -DANDROID_NATIVE_API_LEVEL=24 \
    -DANDROID_NDK_MAJOR=21 \
    -DANDROID_BUILD_ABI_armeabi-v7a=ON \
    -DANDROID_DEPLOY_QT:FILEPATH=~/shared/Qt/5.15.2/android/bin/androiddeployqt \
    -DANDROID_SDK:PATH=~/Android/Sdk \
    -DANDROID_NDK:PATH=~/Android/Sdk/ndk/21.4.7075529 \
    -DBUILD_SHARED_LIBS=OFF

# build
cmake --build . --parallel --config Release --target leafxy

# deploy
~/shared/Qt/5.15.2/android/bin/androiddeployqt \
    --input ./android_deployment_settings.json \
    --output ./android-build \
    --android-platform android-30 \
    --jdk ~/shared/jdk1.8.0_301 \
    --gradle

# install
adb install  ./android-build/build/outputs/apk/debug/android-build-debug.apk
```

## windows host build for android platform

```shell
# config
cmake .. -G "Ninja" ^
    -DQt5_DIR:PATH=C:/Qt/5.15.2/android/lib/cmake/Qt5 ^
    -DCMAKE_TOOLCHAIN_FILE=C:/Users/xgd/AppData/Local/Android/Sdk/ndk/21.4.7075529/build/cmake/android.toolchain.cmake ^
    -DANDROID_PLATFORM=30 ^
    -DANDROID=ON ^
    -DANDROID_ABI=armeabi-v7a ^
    -DJAVA_HOME=C:/Library/Runtime/jdk-1.8.0_291 ^
    -DANDROID_NATIVE_API_LEVEL=24 ^
    -DANDROID_NDK_MAJOR=21 ^
    -DANDROID_BUILD_ABI_armeabi-v7a=ON ^
    -DANDROID_DEPLOY_QT:FILEPATH=C:/Qt/5.15.2/android/bin/androiddeployqt ^
    -DANDROID_SDK:PATH=C:/Users/xgd/AppData/Local/Android/Sdk ^
    -DANDROID_NDK:PATH=C:/Users/xgd/AppData/Local/Android/Sdk/ndk/21.4.7075529  ^
    -DBUILD_SHARED_LIBS=OFF
    
# build
cmake --build . --parallel --config Release --target leafxy

# deploy
C:/Qt/5.15.2/android/bin/androiddeployqt ^
    --input ./android_deployment_settings.json ^
    --output ./android-build ^
    --android-platform android-30 ^
    --jdk C:/Library/Runtime/jdk-1.8.0_291 ^
    --gradle
    
# install
adb install  ./android-build/build/outputs/apk/debug/android-build-debug.apk
```