#----Settings----

# Select type of library: shared/static
TYPE = static

#OPENCL = true

#----------------

CONFIG += console
CONFIG += c++11
CONFIG -= debug_and_release
DEFINES += _CRT_SECURE_NO_WARNINGS

TARGET = libmultifragmentregister
TEMPLATE = lib

# Select type of library: shared/static
contains(TYPE, shared) {
    DEFINES += SHARED_LIBREGISTER_LIBRARY
    DEFINES += SHARED_LIBREGISTER_EXPORT
} else {
    CONFIG += staticlib
}

INCLUDEPATH += \
    $$PWD \
    $$PWD/include \

DEPENDPATH += $$PWD

# Detect architecture and build mode (ARCH = x86/x64, BUILD_MODE = debug/release)
include($$PWD/../detect_arch_and_build_mode.pri)
include($$PWD/../check_qt_version.pri)

SOURCES += \
    src/ImageMetric/timagemetric.cpp \
    src/ImageMetric/tsimplemetric.cpp \
    src/ImageMetric/topenglnormalizedmutualinformationmetric.cpp \
    src/ImageMetric/topenglsquareddifferencesmetric.cpp \
    src/ImageMetric/topenclnormalizedmutualinformationmetric.cpp \
    src/ImageMetric/topenclsquareddifferencesmetric.cpp \
    src/ImageMetric/tcpunormalizedmutualinformationmetric.cpp \
    src/ImageMetric/tcpusquareddifferencesmetric.cpp \
    src/Observer/tobserver.cpp \
    src/Observer/tdefaultobserver.cpp \
    src/VertexMetric/tvertexmetric.cpp \
    src/VertexMetric/tsimplevertexmetric.cpp \
    src/VertexMetric/tsquareddifferencesvertexmetric.cpp \
    src/libmultifragmentregisterabstract.cpp \
    src/VertexMetric/tpoint2pointvertexmetric.cpp \
    src/ImageMetric/tsimplemetricmask.cpp


HEADERS += \
    include/libmultifragmentregister.h \
    include/libmultifragmentregister_global.h \
    include/tbonefragment.h \
    include/txrayview.h \
    include/libmultifragmentregister.hpp \
    include/tbonefragment.hpp \
    include/txrayview.hpp \
    include/ImageMetric/timagemetric.h \
    include/ImageMetric/tsimplemetric.h \
    include/ImageMetric/topenglnormalizedmutualinformationmetric.h \
    include/ImageMetric/topenglsquareddifferencesmetric.h \
    include/ImageMetric/topenclnormalizedmutualinformationmetric.h \
    include/ImageMetric/topenclsquareddifferencesmetric.h \
    include/ImageMetric/tcpunormalizedmutualinformationmetric.h \
    include/ImageMetric/tcpusquareddifferencesmetric.h \
    include/optimisationmodel.h \
    include/Observer/tobserver.h \
    include/Observer/tdefaultobserver.h \
    include/VertexMetric/tvertexmetric.h \
    include/VertexMetric/tsimplevertexmetric.h \
    include/libmultifragmentregisterabstract.h \
    include/VertexMetric/tsquareddifferencesvertexmetric.h \
    include/VertexMetric/tpoint2pointvertexmetric.h \
    include/ImageMetric/tsimplemetricmask.h

# Dependents
include(libmultifragmentregister_dependents.pri)

# Copy built library files to destination
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH\" $(MKDIR) \"$$PWD/lib/$$ARCH\") &
QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/lib/$$ARCH/$$BUILD_MODE\") &
QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.lib\" \"$$PWD\\lib\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.lib\" &

contains(TYPE, shared) {
    QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH\" $(MKDIR) \"$$PWD/bin/$$ARCH\") &
    QMAKE_POST_LINK += ($(CHK_DIR_EXISTS) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\" $(MKDIR) \"$$PWD/bin/$$ARCH/$$BUILD_MODE\") &
    QMAKE_POST_LINK += $${QMAKE_COPY} \"$$OUT_PWD\\$${TARGET}.dll\" \"$$PWD\\bin\\$$ARCH\\$$BUILD_MODE\\$${TARGET}.dll\"
}
