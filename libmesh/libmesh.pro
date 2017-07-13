QT -= core gui

TARGET = libmesh
TEMPLATE = lib

CONFIG += staticlib
CONFIG -= debug_and_release
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += \
    $$PWD \
    $$PWD/include \
    $$PWD/include/lib3d \

DEPENDPATH += $$PWD

# Detect architecture and build mode (ARCH = x86/x64, BUILD_MODE = debug/release)
include($$PWD/../detect_arch_and_build_mode.pri)

HEADERS += \
    include/libmesh.h\
    include/colormap.h \
    include/compute_error.h \
    include/mesh.h \
    include/mesh_run.h \
    include/model_analysis.h \
    include/reporting.h \
    include/xalloc.h \
    include/lib3d/3dmodel.h \
    include/lib3d/block_list.h \
    include/lib3d/debug_print.h \
    include/lib3d/geomutils.h \
    include/lib3d/model_in.h \
    include/lib3d/model_in_ply.h \
    include/lib3d/types.h \

SOURCES += \
    src/colormap.c \
    src/compute_error.c \
    src/mesh_run.c \
    src/model_analysis.c \
    src/reporting.c \
    src/xalloc.c \
    src/lib3d/block_list.c \
    src/lib3d/geomutils.c \
    src/lib3d/model_in.c \
    src/lib3d/model_in_off.c \
    src/lib3d/model_in_ply.c \
    src/lib3d/model_in_raw.c \
    src/lib3d/model_in_smf.c \
    src/lib3d/model_in_vrml_iv.c \

# Copy built library files to destination
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH\" $(MKDIR) \"$$PWD/lib/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\") &
#QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$$BUILD_MODE\\$${TARGET}.lib\" \"$$PWD\\lib\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.lib\" &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.lib\" \"$$PWD\\lib\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.lib\" &
