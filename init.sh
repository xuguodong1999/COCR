
# set 3rdparty as sub modules
# git init .
# git submodule add https://github.com/opencv/opencv.git                  3rdparty/opencv
# git submodule add https://github.com/eigenteam/eigen-git-mirror.git     3rdparty/eigen
# git submodule add https://github.com/Dobiasd/frugally-deep.git          3rdparty/frugally-deep
# git submodule add https://github.com/nlohmann/json                      3rdparty/json
# git submodule add https://github.com/Dobiasd/FunctionalPlus             3rdparty/FunctionalPlus
# git submodule add https://github.com/Tencent/rapidjson.git              3rdparty/rapidjson
# git submodule add https://github.com/openbabel/openbabel.git            3rdparty/openbabel
git submodule init
git submodule update
git submodule foreach --recursive git submodule update
git submodule foreach git checkout master
git submodule foreach git pull