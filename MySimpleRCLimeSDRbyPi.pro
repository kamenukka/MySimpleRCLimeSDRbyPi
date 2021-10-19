QT += quick
QT += widgets
QT += printsupport


linux-g++ | linux-g++-64 | linux-g++-32 {
LIBS +=  -lpthread
CONFIG += c++2a
} else {
CONFIG += c++17
LIBS +=  -lpthread
QMAKE_LFLAGS += -static
LIBS += D:\Qt\Qt5.12.10\Tools\mingw730_64\x86_64-w64-mingw32\lib\libws2_32.a \
     -static D:\Qt\Qt5.12.10\Tools\mingw730_64\x86_64-w64-mingw32\lib\libwinpthread-1.dll \
    -static-libstdc++ -static-libgcc

QMAKE_LFLAGS    = -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc
QMAKE_LFLAGS_EXCEPTIONS_ON = -mthreads
}

SOURCES += main.cpp \
        appcore.cpp \
        tcp/src/TcpServer.cpp \
        tcp/src/TcpClient.cpp \
        treadObject.cpp

HEADERS += \
        appcore.h \
        tcp/hdr/TcpServer.h \
        tcp/hdr/TcpClient.h \
        tcp/hdr/general.h \
        treadObject.h

TRANSLATIONS += \
    MySimpleRCLimeSDRbyPi_ru_RU.ts

RESOURCES += qml.qrc
QTLOCATION_OPENGL_ITEMS = 1

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    main.qml

QMAKE_CXXFLAGS += -Wl,--stack,100000000
QMAKE_CXXFLAGS += -Wl,--heap,100000000
