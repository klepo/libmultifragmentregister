CONFIG += console
CONFIG += c++11
CONFIG -= debug_and_release
DEFINES += _CRT_SECURE_NO_WARNINGS

QT += widgets

QMAKE_LFLAGS += /ignore:4099

TARGET = reduce2d3d
TEMPLATE = app

INCLUDEPATH += \
    $$PWD \
    $$PWD/include \

include($$PWD/../detect_arch_and_build_mode.pri)

include($$PWD/../ssimrenderer.pri)
include($$PWD/../libmultifragmentregister.pri)
include($$PWD/../libmesh.pri)
include($$PWD/reduce2d3d_dependents.pri)

include($$PWD/../detect_arch_and_build_mode.pri)
include($$PWD/../check_qt_version.pri)

SOURCES += src/main.cpp \
    src/xmlparser.cpp \
    src/poseestimator.cpp \
    src/cropestimator.cpp \
    src/hausdorffdistance.cpp

HEADERS += \
    include/xmlparser.h \
    include/poseestimator.h \
    include/cropestimator.h \
    include/hausdorffdistance.h

# Copy built library files to destination
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH\" $(MKDIR) \"$$PWD/bin/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\") &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.exe\" \"$$PWD\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.exe\" &
