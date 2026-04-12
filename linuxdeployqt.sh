#!/bin/bash

ARCH=$(uname --machine)
ARCH_KERNEL=$(uname -r | cut -d '-' -f 2)
ARCH="${ARCH:-x86_64}"
ARCH_KERNEL="${ARCH_KERNEL:-amd64}"

QT_PLUGINS=$(qmake6 -query QT_INSTALL_PLUGINS 2>/dev/null)

WORKSPACE_DIR="$PWD"
BUILD_DIR="$WORKSPACE_DIR/build/Desktop_Qt_6_10_3-Release"

export EXTRA_QT_MODULES="sql;"
export EXTRA_QT_PLUGINS="waylandcompositor"
export EXTRA_PLATFORM_PLUGINS='libqwayland.so;libqxcb.so;libqminimalegl.so;libqminimal.so'
export LD_LIBRARY_PATH="$APPDIR/usr/lib:$LD_LIBRARY_PATH"

if [ ! -d "$BUILD_DIR" ]; then
    echo "error: bin not built" 1>&2
    exit 1
fi

cleanup() {
    rm -rf $TMP_DIR
    rm -rf "$BUILD_DIR/appdir"
    exit
}

trap cleanup EXIT

TMP_DIR=$(mktemp -d '/tmp/linuxdeployqt-XXXXXXX')

get_tool() {
    local author="$1"
    local repository="$2"
    local branch="$3"
    local tool_name="$4"

    local url="https://github.com/$author/$repository/releases/download/$branch/$tool_name-$ARCH.AppImage"

    echo "> wget --quiet --show-progress \"$url\" --output-document \"$TMP_DIR/$tool_name.AppImage\""
    wget --quiet --show-progress "$url" --output-document "$TMP_DIR/$tool_name.AppImage"
    if [ $? -ne 0 ]; then
        exit 1
    fi
    chmod u+x "$TMP_DIR/$tool_name.AppImage"
}

get_tool "linuxdeploy" "linuxdeploy" "continuous" "linuxdeploy"
if [ $? -ne 0 ]; then
    exit 1
fi

get_tool "AppImage" "appimagetool" "continuous" "appimagetool"
if [ $? -ne 0 ]; then
    exit 1
fi

get_tool "linuxdeploy" "linuxdeploy-plugin-qt" "continuous" "linuxdeploy-plugin-qt"
if [ $? -ne 0 ]; then
    exit 1
fi

cd "$BUILD_DIR" || exit 1

echo "> $TMP_DIR/linuxdeploy.AppImage -e ToDo -d ../../ToDo.desktop -i ../../icon.png --appdir ./appdir"
"$TMP_DIR/linuxdeploy.AppImage" -e ToDo -d ../../ToDo.desktop -i ../../icon.png --appdir ./appdir
if [ $? -ne 0 ]; then
    cd appdir || exit 1
    cp /lib/$ARCH-linux-gnu/libxcb.so.1 ./usr/lib/
    mv ./usr/share/icons/hicolor/512x512/apps/icon.png ./usr/share/icons/hicolor/512x512/apps/ToDo.png
    ln -s ./usr/share/icons/hicolor/512x512/apps/ToDo.png ToDo.png
    cd ..

    echo "> $TMP_DIR/linuxdeploy.AppImage -e ToDo -d ../../ToDo.desktop -i ../../icon.png --appdir ./appdir"
    "$TMP_DIR/linuxdeploy.AppImage" -e ToDo -d ../../ToDo.desktop -i ../../icon.png --appdir ./appdir
    if [ $? -ne 0 ]; then
        exit 1
    fi
fi

echo "> $TMP_DIR/linuxdeploy.AppImage --appdir ./appdir --plugin qt --output appimage"
"$TMP_DIR/linuxdeploy.AppImage" --appdir ./appdir --plugin qt --output appimage

cp -r "$QT_PLUGINS" "$BUILD_DIR/appdir/usr/"
if [ $? -ne 0 ]; then
    exit 1
fi
find "$BUILD_DIR/appdir/usr/plugins" -name "*.debug" -type f -delete
if [ $? -ne 0 ]; then
    exit 1
fi

cp /usr/lib/$ARCH-linux-gnu/libxcb-*.so "$BUILD_DIR/appdir/usr/lib/"
cp $QT_PLUGINS/../lib/libQt6*.so.6 "$BUILD_DIR/appdir/usr/lib/"

echo "> $TMP_DIR/appimagetool.AppImage ./appdir"
"$TMP_DIR/appimagetool.AppImage" ./appdir
if [ $? -ne 0 ]; then
    exit 1
fi

echo "done."
exit 0
