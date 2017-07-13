### shared
#DEFINES += SHARED_LIBMESH_LIBRARY
#QMAKE_PRE_LINK += $${QMAKE_COPY} \"$$PWD/../libmesh\\bin\\$$ARCH\\$$BUILD_MODE\\libmesh.dll\" \"$$OUT_PWD\\$$BUILD_MODE\\libmesh.dll\"
###
LIBS += -L"$$PWD/libmesh/lib/$$ARCH/$$BUILD_MODE" -llibmesh
INCLUDEPATH += $$PWD/libmesh/include/
DEPENDPATH += $$PWD/libmesh/include/
PRE_TARGETDEPS += $$PWD/libmesh/lib/$$ARCH/$$BUILD_MODE/libmesh.lib
INCLUDEPATH += $$PWD/libmesh/
