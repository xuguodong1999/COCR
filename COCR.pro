QT += core 3danimation 3dcore 3dextras 3drender

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 big_resources console object_parallel_to_source

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 \
    HAVE_EIGEN3 HAVE_EIGEN \
    DISABLE_WRITE_COMPRESSION \
    HAVE_RAPIDJSON \
    HAVE_SHARED_POINTER \
    HAVE_STATIC_INCHI


INCLUDEPATH += $${PWD}/include


# For OpenBabel

OB_SRC_DIR = $${PWD}/openbabel

INCLUDEPATH += $${OB_SRC_DIR}/include \
    $${OB_SRC_DIR}/include/inchi \
    $${OB_SRC_DIR}/src

INCLUDE_DIRS = charges depict descriptors fingerprints \
    forcefields formats math ops stereo

for(INCLUDE_DIR, INCLUDE_DIRS){
    INCLUDEPATH += $${OB_SRC_DIR}/src/$${INCLUDE_DIR}
}

OB_SRCS = alias.cpp descriptor.cpp mcdlutil.cpp pointgroup.cpp atom.cpp distgeom.cpp molchrg.cpp query.cpp base.cpp elements.cpp mol.cpp rand.cpp bitvec.cpp fingerprint.cpp obconversion.cpp reactionfacade.cpp bond.cpp forcefield.cpp oberror.cpp residue.cpp bondtyper.cpp format.cpp obfunctions.cpp ring.cpp builder.cpp generic.cpp obiter.cpp rotamer.cpp canon.cpp graphsym.cpp obmolecformat.cpp rotor.cpp chains.cpp grid.cpp obutil.cpp spectrophore.cpp chargemodel.cpp griddata.cpp op.cpp tautomer.cpp conformersearch.cpp isomorphism.cpp parsmart.cpp tokenst.cpp kekulize.cpp patty.cpp transform.cpp data.cpp locale.cpp phmodel.cpp typer.cpp data_utilities.cpp matrix.cpp plugin.cpp charges/eem.cpp formats/moldenformat.cpp charges/eqeq.cpp formats/molproformat.cpp charges/fromfile.cpp formats/molreport.cpp charges/gasteiger.cpp formats/mopacformat.cpp charges/mmff94.cpp formats/mpdformat.cpp charges/none.cpp formats/mpqcformat.cpp charges/qeq.cpp formats/msiformat.cpp charges/qtpie.cpp formats/msmsformat.cpp depict/asciipainter.cpp formats/nulformat.cpp formats/nwchemformat.cpp depict/commandpainter.cpp formats/opendxformat.cpp depict/depict.cpp formats/orcaformat.cpp depict/svgpainter.cpp formats/outformat.cpp descriptors/cansmidescriptor.cpp formats/painterformat.cpp descriptors/cmpdfilter.cpp formats/pcmodelformat.cpp descriptors/filters.cpp formats/pdbformat.cpp descriptors/groupcontrib.cpp formats/pdbqtformat.cpp descriptors/inchidescriptor.cpp descriptors/smartsdescriptors.cpp fingerprints/finger2.cpp formats/pointcloudformat.cpp fingerprints/finger3.cpp formats/posformat.cpp fingerprints/fingerecfp.cpp formats/povrayformat.cpp forcefields/forcefieldgaff.cpp formats/pqrformat.cpp forcefields/forcefieldghemical.cpp formats/PQSformat.cpp formats/pwscfformat.cpp forcefields/forcefieldmmff94.cpp formats/qchemformat.cpp forcefields/forcefielduff.cpp formats/reportformat.cpp formats/abinitformat.cpp formats/rinchiformat.cpp formats/acesformat.cpp formats/rsmiformat.cpp formats/acrformat.cpp formats/rxnformat.cpp formats/adfformat.cpp formats/shelxformat.cpp formats/alchemyformat.cpp formats/siestaformat.cpp formats/amberformat.cpp formats/smilesformat.cpp formats/aoforceformat.cpp formats/smileyformat.cpp formats/APIInterface.cpp formats/stlformat.cpp formats/asciiformat.cpp formats/svgformat.cpp formats/balstformat.cpp formats/textformat.cpp formats/bgfformat.cpp formats/thermoformat.cpp formats/boxformat.cpp formats/tinkerformat.cpp formats/cacaoformat.cpp formats/titleformat.cpp formats/cacheformat.cpp formats/turbomoleformat.cpp formats/carformat.cpp formats/unichemformat.cpp formats/castepformat.cpp formats/vaspformat.cpp formats/cccformat.cpp formats/viewmolformat.cpp formats/chem3dformat.cpp formats/wlnformat.cpp formats/chemdrawcdx.cpp formats/wln-nextmove.cpp formats/chemdrawct.cpp formats/xedformat.cpp formats/chemkinformat.cpp formats/xsfformat.cpp formats/chemtoolformat.cpp \
#    formats/xtcformat.cpp \
    formats/cifformat.cpp formats/xyzformat.cpp formats/cofformat.cpp formats/yasaraformat.cpp formats/confabreport.cpp formats/zindoformat.cpp formats/copyformat.cpp math/align.cpp formats/crkformat.cpp math/matrix3x3.cpp formats/crystal09format.cpp math/spacegroup.cpp formats/CSRformat.cpp math/transform3d.cpp formats/cssrformat.cpp math/vector3.cpp formats/daltonformat.cpp ops/addfilename.cpp formats/dlpolyformat.cpp ops/addinindex.cpp formats/dmolformat.cpp ops/addnonpolarh.cpp ops/addpolarh.cpp formats/exyzformat.cpp ops/canonical.cpp formats/fastaformat.cpp ops/changecell.cpp formats/fastsearchformat.cpp ops/conformer.cpp formats/fchkformat.cpp ops/delnonpolarh.cpp formats/featformat.cpp ops/delpolarh.cpp formats/fhformat.cpp ops/fillUC.cpp formats/fhiaimsformat.cpp ops/forcefield.cpp formats/fingerprintformat.cpp ops/gen2D.cpp formats/fpsformat.cpp ops/gen3d.cpp formats/freefracformat.cpp ops/largest.cpp formats/gamessformat.cpp ops/loader.cpp formats/gamessukformat.cpp ops/neutralize.cpp formats/gausscubeformat.cpp formats/gaussformat.cpp ops/opalign.cpp formats/gausszmatformat.cpp formats/genbankformat.cpp ops/ophighlight.cpp formats/getinchi.cpp ops/opisomorph.cpp formats/ghemicalformat.cpp ops/opsplit.cpp formats/groformat.cpp ops/optransform.cpp formats/gromos96format.cpp ops/partialcharges.cpp formats/gulpformat.cpp ops/readconformers.cpp formats/hinformat.cpp ops/sort.cpp formats/inchiformat.cpp ops/unique.cpp formats/jaguarformat.cpp ops/xout.cpp formats/lmpdatformat.cpp stereo/cistrans.cpp formats/lpmdformat.cpp stereo/facade.cpp stereo/gen3dstereohelper.cpp formats/MCDLformat.cpp stereo/perception.cpp formats/mdffformat.cpp stereo/squareplanar.cpp formats/mdlformat.cpp stereo/stereo.cpp formats/mmcifformat.cpp stereo/tetrahedral.cpp formats/mmodformat.cpp stereo/tetranonplanar.cpp formats/MNAformat.cpp stereo/tetraplanar.cpp formats/mol2format.cpp formats/ArrayList.c formats/libinchi/ichi_bns.c formats/libinchi/ichirvr1.c formats/libinchi/ichican2.c formats/libinchi/ichirvr2.c formats/libinchi/ichicano.c formats/libinchi/ichirvr3.c formats/libinchi/ichicans.c formats/libinchi/ichirvr4.c formats/libinchi/ichi_io.c formats/libinchi/ichirvr5.c formats/libinchi/ichiisot.c formats/libinchi/ichirvr6.c formats/libinchi/ichilnct.c formats/libinchi/ichirvr7.c formats/libinchi/ichimak2.c formats/libinchi/ichisort.c formats/libinchi/ichimake.c formats/libinchi/ichister.c formats/libinchi/ichimap1.c formats/libinchi/ichitaut.c formats/libinchi/ichimap2.c formats/libinchi/ikey_base26.c formats/libinchi/ichimap4.c formats/libinchi/ikey_dll.c formats/libinchi/ichinorm.c formats/libinchi/inchi_dll_a2.c formats/libinchi/ichiparm.c formats/libinchi/inchi_dll_a.c formats/libinchi/ichiprt1.c formats/libinchi/inchi_dll.c formats/libinchi/ichiprt2.c formats/libinchi/inchi_dll_main.c formats/libinchi/ichiprt3.c formats/libinchi/runichi.c formats/libinchi/ichiqueu.c formats/libinchi/sha2.c formats/libinchi/ichiread.c formats/libinchi/strutil.c formats/libinchi/ichiring.c formats/libinchi/util.c formats/json/chemdoodlejsonformat.cpp formats/json/pubchemjsonformat.cpp

for(OB_SRC,OB_SRCS){
    SOURCES += $${OB_SRC_DIR}/src/$${OB_SRC}
}

msvc:{
# for debug build, compile oencv and openbabel from source
#    QMAKE_CFLAGS_DEBUG += -MTd
#    QMAKE_CXXFLAGS_DEBUG += -MTd

    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT

    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp

    RC_FILE = $${PWD}/src/resources/COCR.rc

    MY_MSVC_LIB_DIR = $${PWD}/msvc_x64_lib

    INCLUDEPATH += $${MY_MSVC_LIB_DIR}/include

    LIB_NAME_LIST = openbabel-3.lib \
#        opencv_highgui451.lib \
        opencv_imgcodecs451.lib opencv_imgproc451.lib \
        opencv_dnn451.lib opencv_core451.lib \
        ade.lib libopenjp2.lib libwebp.lib IlmImf.lib \
        libpng.lib  ittnotify.lib zlib.lib \
        libprotobuf.lib libjpeg-turbo.lib libtiff.lib

    for(LIB_NAME, LIB_NAME_LIST){
        LIBS += $${MY_MSVC_LIB_DIR}/x64/$${LIB_NAME}
    }
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

INCLUDE_LIST = alkane_graph.hpp hw_script.hpp mol3dwindow.hpp \
    qt_sketchwidget.hpp  atom.hpp hw_stroke.hpp openbabel_util.hpp \
    soso17_converter.hpp bond.hpp mol.hpp opencv_util.hpp std_util.hpp \
    bond_type.hpp mol_holder.hpp yolo.hpp cocr_types.hpp mol_op.hpp \
    qt_mainwindow.hpp yolo_object.hpp colors.hpp mol_util.hpp \
    qt_mol3deditor.hpp yolo_opencv.hpp fraction.hpp mol3d.hpp \
    qt_molfileeditor.hpp hw_base.hpp mol3d_builder.hpp \
    qt_ocrthread.hpp qt_util.hpp

SRC_LIST = atom.cpp main.cpp mol3dwindow.cpp qt_ocrthread.cpp \
    bond.cpp mol.cpp openbabel_util.cpp qt_sketchwidget.cpp \
    cocr_types.cpp mol_holder.cpp opencv_util.cpp \
    soso17_converter.cpp colors.cpp mol_op.cpp std_util.cpp \
    hw_base.cpp mol_util.cpp qt_mainwindow.cpp yolo.cpp \
    hw_script.cpp mol3d.cpp qt_mol3deditor.cpp \
    yolo_opencv.cpp hw_stroke.cpp mol3d_builder.cpp \
    qt_molfileeditor.cpp qt_util.cpp

UI_LIST = qt_mainwindow.ui qt_mol3deditor.ui qt_molfileeditor.ui

for(INCLUDE_FILE,INCLUDE_LIST){
    HEADERS += $${PWD}/src/$${INCLUDE_FILE}
}
for(SRC_FILE,SRC_LIST){
    SOURCES += $${PWD}/src/$${SRC_FILE}
}
for(UI_FILE,UI_LIST){
    FORMS += $${PWD}/src/$${UI_FILE}
}

RESOURCES += $${PWD}/src/resources/res.qrc \
    $${PWD}/src/resources/big_res.qrc

TRANSLATIONS += $${PWD}/src/resources/COCR_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
