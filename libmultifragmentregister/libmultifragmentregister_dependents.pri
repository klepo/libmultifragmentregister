INCLUDEPATH += \
    "$$PWD/3rdparty/dlib/include" \

LIBS += \
    -lShell32 \
    -lAdvapi32 \
    -L"$$PWD/3rdparty/dlib/lib/$$ARCH/$$BUILD_MODE" \
    -ldlib \
    -lkernel32 \
    -luser32 \
    -lgdi32 \
    -lwinspool \
    -lshell32 \
    -lole32 \
    -loleaut32 \
    -luuid \
    -lcomdlg32 \
    -ladvapi32 \


include($$PWD/../ssimrenderer.pri)
