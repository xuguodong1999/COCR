REM set 3rdparty as sub modules
REM git init .
REM git submodule add https://github.com/opencv/opencv.git                  3rdparty/opencv
REM git submodule add https://github.com/eigenteam/eigen-git-mirror.git     3rdparty/eigen
REM git submodule add https://github.com/Dobiasd/frugally-deep.git          3rdparty/frugally-deep
REM git submodule add https://github.com/nlohmann/json                      3rdparty/json
REM git submodule add https://github.com/Dobiasd/FunctionalPlus             3rdparty/FunctionalPlus
REM git submodule add https://github.com/Tencent/rapidjson.git              3rdparty/rapidjson
REM git submodule add https://github.com/openbabel/openbabel.git            3rdparty/openbabel
REM git submodule add https://github.com/madler/zlib.git                    3rdparty/zlib
REM git submodule add https://github.com/GNOME/libxml2.git                  3rdparty/libxml2
git submodule init
git submodule update
git submodule foreach --recursive git submodule update
