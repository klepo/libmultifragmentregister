INCLUDEPATH += \
    "$$PWD/3rdparty/opencv/include" \

CONFIG(debug, debug|release) {
    LIBS += -L"$$PWD/3rdparty/opencv/lib/$$ARCH/$$BUILD_MODE" \
            -lopencv_world300d \
            -lopencv_ts300d \

    QMAKE_POST_LINK += $${QMAKE_COPY} \"$$PWD\\3rdparty\\opencv\\bin\\$$ARCH\\$$BUILD_MODE\\opencv_world300d.dll\" \"$$OUT_PWD\\opencv_world300d.dll\" &
} else {
    LIBS += -L"$$PWD/3rdparty/opencv/lib/$$ARCH/$$BUILD_MODE" \
            -lopencv_world300 \
            -lopencv_ts300 \

    QMAKE_POST_LINK += $${QMAKE_COPY} \"$$PWD\\3rdparty\\opencv\\bin\\$$ARCH\\$$BUILD_MODE\\opencv_world300.dll\" \"$$OUT_PWD\\opencv_world300.dll\" &
}
