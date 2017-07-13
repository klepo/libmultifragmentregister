include($$PWD/libmultifragmentregister/libmultifragmentregister_dependents.pri)

### shared
#DEFINES += SHARED_libmultifragmentregister_LIBRARY
#QMAKE_PRE_LINK += $${QMAKE_COPY} \"$$PWD/../libmultifragmentregister\\bin\\$$ARCH\\$$BUILD_MODE\\libmultifragmentregister.dll\" \"$$OUT_PWD\\$$BUILD_MODE\\libmultifragmentregister.dll\"
###
LIBS += -L"$$PWD/libmultifragmentregister/lib/$$ARCH/$$BUILD_MODE" -llibmultifragmentregister #/NODEFAULTLIB:libcmt.lib
INCLUDEPATH += $$PWD/libmultifragmentregister/include
DEPENDPATH += $$PWD/libmultifragmentregister/include
PRE_TARGETDEPS += $$PWD/libmultifragmentregister/lib/$$ARCH/$$BUILD_MODE/libmultifragmentregister.lib
INCLUDEPATH += $$PWD/libmultifragmentregister/
