#ifndef _COCR_H_
#define _COCR_H_

#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <QDebug>

#define LogName(value) std::cout << (#value) << "=" << (value) << std::endl;
#define LogLine(value) std::cout << (value) << std::endl;
#define LogValue(value) std::cout << (value);
#define LogPressKey2Continue() std::cout << std::endl << "press any key to continue..." << std::endl;
namespace cocr {
    inline const float &eps() {
        return 0.0000000000000001;
    }

    inline const float &epsNeg() {
        return -0.0000000000000001;
    }

    enum class ErrorCode {
        NormalExec,
        OBForceFieldMissing,
        OBFormatMissing,
        OBReadError,
        InputDirMissing,
        OutputDirMissing,
        MoveFileFailed,
        CreateFileFailed,
        DeleteFileFailed,
        CvDnnError,

        CEPreProcessFailed,
        CELexicalAnalysisFailed,
        CEUnknownLetter,
        CEUnknownChar,
        CEElementFailed,
        CEMissingValence,
        CEDivElementFailed,
        CEFormatError,
    };

    template<typename T>
    inline T mod32(const T &t) {
        return t - t % 32;
    }

    template<typename T>
    inline void show(const T &t, std::ostream &out = std::cout) {
        out << t << std::endl;
    }

    template<typename T>
    inline void show(const std::vector<std::vector<T>> &arr2d, std::ostream &out = std::cout) {
        out << "[" << std::endl;
        for (auto &arr : arr2d) {
            out << " [";
            for (auto &ele : arr) {
                out << ele << ",";
            }
            out << "]" << std::endl;
        }
        out << "]" << std::endl;
    }

    template<typename T>
    inline void show(const std::vector<T> &arr, std::ostream &out = std::cout) {
        out << "[";
        for (auto &ele : arr) {
            out << ele << ",";
        }
        out << "]" << std::endl;
    }

    template<typename T>
    inline void show(const std::list<T> &arr, std::ostream &out = std::cout) {
        out << "[";
        for (auto &ele : arr) {
            out << ele << ",";
        }
        out << "]" << std::endl;
    }

    inline void removeAllSubStr(std::string &target, const std::string &subStr) {
        while (true) {
            auto pos = target.find(subStr);
            if (pos == std::string::npos) {
                return;
            }
            target.erase(pos, subStr.length());
        }
    }

    inline void replaceAllSubStr(std::string &target, const std::string &subSrc, const std::string &subDst) {
        for (auto pos = 0; pos != std::string::npos; pos += subDst.length()) {
            pos = target.find(subSrc, pos);
            if (pos != std::string::npos) {
                target.replace(pos, subSrc.length(), subDst);
            } else {
                break;
            }
        }
    }
}
#endif
