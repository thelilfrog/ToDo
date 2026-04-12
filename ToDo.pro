QT += widgets sql

CONFIG += c++17

win32:VERSION = 0.0.1.0 # major.minor.patch.build
else:VERSION = 0.0.1    # major.minor.patch

DEFINES += APP_VERSION=\"\\\"$${VERSION}.preview1\\\"\"
TMP_APP_ARCH = APP_ARCH=\"\\\"unknown\\\"\"

# remove possible other optimization flags
win32 {
    message("Build for Windows")
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE *= -O2
    equals(QMAKE_TARGET.arch, arm64) {
        message("CPU Architecture : aarch64")
        TMP_APP_ARCH = APP_ARCH=\"\\\"arm64\\\"\"
    }
    equals(QMAKE_TARGET.arch, x86_64) {
        message("CPU Architecture : x64")
        QMAKE_CXXFLAGS_RELEASE += -favor:INTEL64
        TMP_APP_ARCH = APP_ARCH=\"\\\"x64\\\"\"
    }
    RC_ICONS = icon.ico
    QMAKE_TARGET_COMPANY = "Aurelie Delhaie"
    QMAKE_TARGET_PRODUCT = "ToDo"
    QMAKE_TARGET_DESCRIPTION = "ToDo"
}

macx {
    message("Build for macOS")
    ICON = icon.icns
    QMAKE_INFO_PLIST = Info.plist

    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    QMAKE_CXXFLAGS += -Werror

    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -O3

    QMAKE_CXXFLAGS_RELEASE += -march=armv8.6-a+fp16+simd
    QMAKE_APPLE_DEVICE_ARCHS = arm64

    TMP_APP_ARCH = APP_ARCH=\"\\\"arm64\\\"\"
}

linux-* {
    message("Build for Linux")

    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    QMAKE_CXXFLAGS += -Werror

    equals(QMAKE_HOST.arch, aarch64) {
        message("CPU Architecture : aarch64")
        QMAKE_CXXFLAGS_RELEASE += -mtune=armv8.6-a+fp16+simd
        TMP_APP_ARCH = APP_ARCH=\"\\\"aarch64\\\"\"
    }
    equals(QMAKE_HOST.arch, amd64) {
        message("CPU Architecture : amd64")
        QMAKE_CXXFLAGS_RELEASE += -march=tigerlake
        QMAKE_CXXFLAGS_RELEASE += -mtune=intel
        TMP_APP_ARCH = APP_ARCH=\"\\\"amd64\\\"\"
    }
    QMAKE_CXXFLAGS_RELEASE *= -O3
}

DEFINES += $$TMP_APP_ARCH

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/noteservice.cpp \
    src/gui/dialog/about/aboutdialog.cpp \
    src/gui/dialog/input/inputdialog.cpp \
    src/obj/list.cpp \
    src/core/listservice.cpp \
    src/main.cpp \
    src/gui/mainwindow.cpp \
    src/obj/note.cpp

HEADERS += \
    src/core/noteservice.h \
    src/gui/dialog/about/aboutdialog.h \
    src/gui/dialog/input/inputdialog.h \
    src/obj/list.h \
    src/core/listservice.h \
    src/gui/mainwindow.h \
    src/obj/note.h

FORMS += \
    src/gui/dialog/about/aboutdialog.ui \
    src/gui/dialog/input/inputdialog.ui \
    src/gui/mainwindow.ui

TRANSLATIONS += \
    ToDo_en_150.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/resources.qrc
