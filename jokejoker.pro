QT += core 3dcore 3dextras 3drender

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 big_resources console object_parallel_to_source

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

INCLUDEPATH += $${PWD}/include $${PWD}/src \
    $${PWD}/openbabel/include \
    $${PWD}/openbabel/include/inchi

OPENBABEL_HEADERS = elementtable.h formats/mdlvalence.h formats/smilesvalence.h formats/smiley.h rand.h stereo/gen3dstereohelper.h stereo/stereoutil.h ops/deferred.h ops/opisomorph.h forcefields/forcefieldmmff94.h forcefields/forcefieldgaff.h forcefields/forcefieldghemical.h forcefields/forcefielduff.h charges/qeq.h charges/qtpie.h charges/eqeq.h

for(OPENBABEL_HEADER,OPENBABEL_HEADERS){
    HEADERS += $${PWD}/openbabel/src/$${OPENBABEL_HEADER}
}

OPENBABEL_SRCS = data.cpp grid.cpp typer.cpp depict/depict.cpp depict/commandpainter.cpp depict/asciipainter.cpp depict/svgpainter.cpp query.cpp forcefield.cpp confsearch.cpp rotor.cpp plugin.cpp descriptors/inchidescriptor.cpp descriptors/filters.cpp descriptors/groupcontrib.cpp descriptors/cmpdfilter.cpp descriptors/cansmidescriptor.cpp descriptors/smartsdescriptors.cpp mol.cpp locale.cpp format.cpp patty.cpp bitvec.cpp distgeom.cpp chargemodel.cpp transform.cpp obmolecformat.cpp conformersearch.cpp formats/daltonformat.cpp formats/vaspformat.cpp formats/chem3dformat.cpp formats/freefracformat.cpp formats/mopacformat.cpp formats/groformat.cpp formats/moldenformat.cpp formats/nwchemformat.cpp formats/confabreport.cpp formats/rsmiformat.cpp formats/gausszmatformat.cpp formats/cssrformat.cpp formats/cacheformat.cpp formats/balstformat.cpp formats/acesformat.cpp formats/pcmodelformat.cpp formats/pointcloudformat.cpp formats/inchiformat.cpp formats/fastaformat.cpp formats/cofformat.cpp formats/MCDLformat.cpp formats/chemdrawcdx.cpp formats/orcaformat.cpp formats/stlformat.cpp formats/asciiformat.cpp formats/fhformat.cpp formats/cccformat.cpp formats/copyformat.cpp formats/turbomoleformat.cpp formats/MNAformat.cpp formats/pwscfformat.cpp formats/nulformat.cpp formats/rinchiformat.cpp formats/acrformat.cpp formats/dlpolyformat.cpp formats/crystal09format.cpp formats/msmsformat.cpp formats/lmpdatformat.cpp formats/chemdrawct.cpp formats/hinformat.cpp formats/yasaraformat.cpp formats/siestaformat.cpp formats/crkformat.cpp formats/fingerprintformat.cpp formats/smilesformat.cpp formats/CSRformat.cpp formats/povrayformat.cpp formats/boxformat.cpp formats/amberformat.cpp formats/adfformat.cpp formats/carformat.cpp formats/mpdformat.cpp formats/fastsearchformat.cpp formats/jaguarformat.cpp formats/painterformat.cpp formats/mmcifformat.cpp formats/fpsformat.cpp formats/molproformat.cpp formats/textformat.cpp formats/qchemformat.cpp formats/unichemformat.cpp formats/gaussformat.cpp formats/fchkformat.cpp formats/mdlformat.cpp formats/pdbformat.cpp formats/chemtoolformat.cpp formats/json/pubchemjsonformat.cpp formats/json/chemdoodlejsonformat.cpp formats/castepformat.cpp formats/dmolformat.cpp formats/chemkinformat.cpp formats/wln-nextmove.cpp formats/aoforceformat.cpp formats/tinkerformat.cpp formats/gromos96format.cpp formats/alchemyformat.cpp formats/titleformat.cpp formats/bgfformat.cpp formats/PQSformat.cpp formats/getinchi.cpp formats/pdbqtformat.cpp formats/gamessformat.cpp formats/outformat.cpp formats/thermoformat.cpp formats/zindoformat.cpp formats/msiformat.cpp formats/mol2format.cpp formats/lpmdformat.cpp formats/abinitformat.cpp formats/genbankformat.cpp formats/gulpformat.cpp formats/reportformat.cpp formats/exyzformat.cpp formats/shelxformat.cpp formats/ghemicalformat.cpp formats/opendxformat.cpp formats/mdffformat.cpp formats/mmodformat.cpp formats/xsfformat.cpp formats/gamessukformat.cpp formats/svgformat.cpp formats/smileyformat.cpp formats/fhiaimsformat.cpp formats/molreport.cpp formats/wlnformat.cpp formats/xyzformat.cpp formats/rxnformat.cpp formats/mpqcformat.cpp formats/viewmolformat.cpp formats/gausscubeformat.cpp formats/cacaoformat.cpp formats/featformat.cpp formats/APIInterface.cpp formats/xedformat.cpp formats/cifformat.cpp formats/pqrformat.cpp formats/posformat.cpp op.cpp kekulize.cpp oberror.cpp descriptor.cpp mcdlutil.cpp math/transform3d.cpp math/spacegroup.cpp math/vector3.cpp math/matrix3x3.cpp math/align.cpp pointgroup.cpp tokenst.cpp obfunctions.cpp alias.cpp fingerprints/finger2.cpp fingerprints/finger3.cpp fingerprints/fingerecfp.cpp stereo/tetranonplanar.cpp stereo/gen3dstereohelper.cpp stereo/cistrans.cpp stereo/stereo.cpp stereo/tetrahedral.cpp stereo/squareplanar.cpp stereo/tetraplanar.cpp stereo/facade.cpp stereo/perception.cpp data_utilities.cpp generic.cpp ops/conformer.cpp ops/ophighlight.cpp ops/delnonpolarh.cpp ops/opconfab.cpp ops/unique.cpp ops/partialcharges.cpp ops/fillUC.cpp ops/optransform.cpp ops/readconformers.cpp ops/changecell.cpp ops/gen2D.cpp ops/neutralize.cpp ops/canonical.cpp ops/loader.cpp ops/addpolarh.cpp ops/delpolarh.cpp ops/sort.cpp ops/addnonpolarh.cpp ops/xout.cpp ops/opsplit.cpp ops/openergy.cpp ops/addinindex.cpp ops/gen3d.cpp ops/opalign.cpp ops/largest.cpp ops/opisomorph.cpp ops/addfilename.cpp obutil.cpp isomorphism.cpp canon.cpp phmodel.cpp atom.cpp reactionfacade.cpp bondtyper.cpp elements.cpp ring.cpp forcefields/forcefieldgaff.cpp forcefields/forcefielduff.cpp forcefields/forcefieldghemical.cpp forcefields/forcefieldmmff94.cpp bond.cpp builder.cpp griddata.cpp tautomer.cpp obconversion.cpp spectrophore.cpp rotamer.cpp parsmart.cpp molchrg.cpp graphsym.cpp obiter.cpp charges/mmff94.cpp charges/qtpie.cpp charges/eqeq.cpp charges/eem.cpp charges/fromfile.cpp charges/qeq.cpp charges/gasteiger.cpp charges/none.cpp fingerprint.cpp matrix.cpp chains.cpp base.cpp rand.cpp residue.cpp \
formats/libinchi/ichiqueu.c formats/libinchi/ichinorm.c formats/libinchi/ichi_io.c formats/libinchi/ichisort.c formats/libinchi/ichiparm.c formats/libinchi/ikey_base26.c formats/libinchi/ichiring.c formats/libinchi/ichister.c formats/libinchi/sha2.c formats/libinchi/ichimap4.c formats/libinchi/ichirvr2.c formats/libinchi/ichimak2.c formats/libinchi/ichilnct.c formats/libinchi/ichirvr4.c formats/libinchi/ichirvr1.c formats/libinchi/ichiprt1.c formats/libinchi/ikey_dll.c formats/libinchi/ichimake.c formats/libinchi/util.c formats/libinchi/ichiread.c formats/libinchi/runichi.c formats/libinchi/ichimap2.c formats/libinchi/inchi_dll.c formats/libinchi/ichirvr5.c formats/libinchi/inchi_dll_a.c formats/libinchi/ichican2.c formats/libinchi/ichiprt3.c formats/libinchi/ichicans.c formats/libinchi/ichirvr3.c formats/libinchi/strutil.c formats/libinchi/inchi_dll_main.c formats/libinchi/ichimap1.c formats/libinchi/ichiisot.c formats/libinchi/ichi_bns.c formats/libinchi/ichirvr7.c formats/libinchi/ichicano.c formats/libinchi/ichirvr6.c formats/libinchi/ichitaut.c formats/libinchi/inchi_dll_a2.c formats/libinchi/ichiprt2.c

for(OPENBABEL_SRC, OPENBABEL_SRCS){
    SOURCES += $${PWD}/openbabel/src/$${OPENBABEL_SRC}
}

msvc:{
# for debug build, compile oencv and openbabel from source
#    QMAKE_CFLAGS_DEBUG += -MTd
#    QMAKE_CXXFLAGS_DEBUG += -MTd

    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT

    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp

    RC_FILE = $${PWD}/resources/app.rc

    OPENCV_LIB_DIR = $${PWD}/opencv
    NCNN_LIB_DIR = $${PWD}/ncnn

    INCLUDEPATH += $${PWD}/data_gen \
        $${OPENCV_LIB_DIR}/include $${NCNN_LIB_DIR}/include
    LIB_NAME_LIST = opencv_highgui451.lib \
        opencv_imgcodecs451.lib opencv_imgproc451.lib \
        opencv_dnn451.lib opencv_core451.lib \
        zlib.lib libprotobuf.lib libjpeg-turbo.lib libpng.lib
    for(LIB_NAME, LIB_NAME_LIST){
        LIBS += $${OPENCV_LIB_DIR}/x64/vc16/staticlib/$${LIB_NAME}
    }
    LIBS += $${NCNN_LIB_DIR}/lib/ncnn.lib gdi32.lib user32.Lib advAPI32.Lib comdlg32.lib
}

unix:!android:{
    # TODO: set your opencv dir here.
    # >=4.5.0 is required, 4.4.0 is not ok.
    MY_OPENCV_DIR=/home/xgd/shared/opencv4.5.1
    INCLUDEPATH += $${MY_OPENCV_DIR}/include/opencv4
    LIBS += -L$${MY_OPENCV_DIR}/lib \
        -lopencv_imgcodecs -lopencv_imgproc -lopencv_dnn \
        -lopencv_core -lopencv_highgui
}
android:{
    OPENCV_SDK_DIR = $${PWD}/android/OpenCV-android-sdk

    INCLUDEPATH += $${OPENCV_SDK_DIR}/sdk/native/jni/include

    OPENCV_STATICLIB_DIR = $${OPENCV_SDK_DIR}/sdk/native/staticlibs

    OPENCV_3RDPARTY_DIR = $${OPENCV_SDK_DIR}/sdk/native/3rdparty/libs

    OPENCV_NATIVE_LIST = libopencv_dnn.a libopencv_highgui.a \
        libopencv_imgcodecs.a libopencv_imgproc.a libopencv_core.a

    for(LIB_NAME, OPENCV_NATIVE_LIST){
        LIBS += \
            $${OPENCV_STATICLIB_DIR}/$${ANDROID_TARGET_ARCH}/$${LIB_NAME}
    }

    OPENCV_3RDPARTY_LIST = libquirc.a \
        libade.a libIlmImf.a liblibopenjp2.a liblibjpeg-turbo.a \
        liblibpng.a liblibprotobuf.a liblibtiff.a liblibwebp.a \
        libcpufeatures.a libittnotify.a libtbb.a

    equals(ANDROID_TARGET_ARCH, x86){
        OPENCV_3RDPARTY_LIST += libippiw.a libippicv.a
    }
    equals(ANDROID_TARGET_ARCH, x86_64){
        OPENCV_3RDPARTY_LIST += libippiw.a libippicv.a
    }
    equals(ANDROID_TARGET_ARCH, armeabi-v7a){
        OPENCV_3RDPARTY_LIST += libtegra_hal.a
    }
    equals(ANDROID_TARGET_ARCH, arm64-v8a) {
        OPENCV_3RDPARTY_LIST += libtegra_hal.a
    }

    for(LIB_NAME, OPENCV_3RDPARTY_LIST){
        LIBS += $${OPENCV_3RDPARTY_DIR}/$${ANDROID_TARGET_ARCH}/$${LIB_NAME}
    }

}

INCLUDE_LIST = graph_composer.hpp mainwindow.h object_detection.hpp text_recognition.hpp ocr_thread.hpp

SRC_LIST = main.cpp graph_composer.cpp mainwindow.cpp object_detection.cpp text_recognition.cpp ocr_thread.cpp

UI_LIST = mainwindow.ui

for(INCLUDE_FILE,INCLUDE_LIST){
    HEADERS += $${PWD}/app/$${INCLUDE_FILE}
}
for(SRC_FILE,SRC_LIST){
    SOURCES += $${PWD}/app/$${SRC_FILE}
}
for(UI_FILE,UI_LIST){
    FORMS += $${PWD}/app/$${UI_FILE}
}

SOURCES += $${PWD}/data_gen/timer.cpp

RESOURCES += $${PWD}/resources/big_res.qrc \
    $${PWD}/resources/obdata.qrc

TRANSLATIONS += $${PWD}/resources/jokejoker_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
