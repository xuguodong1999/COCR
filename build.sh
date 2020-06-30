
# config rapidjson
cd 3rdparty/rapidjson && git checkout v1.1.0 && cd ../..

mkdir -p lib/rapidjson build/rapidjson && cd build/rapidjson

cmake ../../3rdparty/rapidjson -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/rapidjson -DBUILD_TESTING=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF

cmake --build . -j 8 --target install --config Release && cd ../..

# config eigen3
cd 3rdparty/eigen && git checkout 3.3.7 && cd ../..

mkdir -p lib/eigen build/eigen && cd build/eigen

cmake ../../3rdparty/eigen -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/eigen -DBUILD_TESTING=OFF 

cmake --build . -j 8 --target install --config Release && cd ../..


# config json for frugally-deep
cd 3rdparty/json && git checkout v3.7.3 && cd ../..

mkdir -p lib/json build/json && cd build/json

cmake ../../3rdparty/json  -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/json 

cmake --build . -j 8 --target install --config Release && cd ../..

# config FunctionalPlus for frugally-deep
cd 3rdparty/FunctionalPlus && git checkout v0.2.7-p0 && cd ../..

mkdir -p lib/FunctionalPlus build/FunctionalPlus && cd build/FunctionalPlus

cmake ../../3rdparty/FunctionalPlus  -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/FunctionalPlus 

cmake --build . -j 8 --target install --config Release && cd ../..


# config frugally-deep
cd 3rdparty/frugally-deep && git checkout v0.14.3-p0 && cd ../..

mkdir -p lib/frugally-deep build/frugally-deep && cd build/frugally-deep

cmake ../../3rdparty/frugally-deep  -DFunctionalPlus_DIR=../../lib/FunctionalPlus/lib/cmake/FunctionalPlus -Dnlohmann_json_DIR=../../lib/json/lib/cmake/nlohmann_json -DEigen3_DIR=../../lib/eigen/share/eigen3/cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/frugally-deep 

cmake --build . -j 8 --target install --config Release && cd ../..



# config openbabel
cd 3rdparty/openbabel && git checkout openbabel-3-0-0 && cd ../..

mkdir -p lib/openbabel build/openbabel && cd build/openbabel

cmake ../../3rdparty/openbabel -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../lib/openbabel -DBUILD_TESTING=OFF -DRAPIDJSON_INCLUDE_DIRS=../../lib/rapidjson/include/rapidjson -DBUILD_GUI=OFF -DWITH_INCHI=OFF -DWITH_STATIC_INCHI=OFF -DOPENBABEL_USE_SYSTEM_INCHI=OFF

cmake --build . -j 8 --target install --config Release && cd ../..

# config opencv4
cd 3rdparty/opencv && git checkout 4.3.0 && cd ../..

mkdir -p lib/opencv build/opencv && cd build/opencv

cmake ../../3rdparty/opencv -DCMAKE_INSTALL_PREFIX=../../lib/opencv -DBUILD_opencv_python_tests=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_TESTS=OFF -DBUILD_PREF_TESTS=OFF -DWITH_ADE=OFF -DWITH_FFMPEG=OFF -DWITH_IPP=OFF

cmake --build . -j 8 --target install --config Release && cd ../..
