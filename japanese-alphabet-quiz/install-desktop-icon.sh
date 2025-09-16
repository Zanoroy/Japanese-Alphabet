#!/bin/bash
# install-desktop-icon.sh
# Installs app icons of multiple sizes and the .desktop file for KDE/Plasma and other Linux desktops.
# Requires: ImageMagick (for convert)

set -e

APP_NAME="japanese-alphabet-quiz"
ICON_BASENAME="appicon"
ICON_SRC="$(dirname "$0")/$ICON_BASENAME.png"
DESKTOP_FILE_SRC="$(dirname "$0")/$APP_NAME.desktop"

ICON_SIZES=(16 24 32 48 64 128 256 512)
ICON_THEME_DIR="$HOME/.local/share/icons/hicolor"
DESKTOP_DIR="$HOME/.local/share/applications"

if ! command -v convert &>/dev/null; then
    echo "Error: ImageMagick 'convert' is required. Please install it (e.g., sudo apt install imagemagick)."
    exit 1
fi

if [ ! -f "$ICON_SRC" ]; then
    echo "Error: Source icon $ICON_SRC not found."
    exit 1
fi

mkdir -p "$DESKTOP_DIR"

for size in "${ICON_SIZES[@]}"; do
    ICON_DEST_DIR="$ICON_THEME_DIR/${size}x${size}/apps"
    mkdir -p "$ICON_DEST_DIR"
    convert "$ICON_SRC" -resize ${size}x${size} "$ICON_DEST_DIR/$APP_NAME.png"
    echo "Installed icon: $ICON_DEST_DIR/$APP_NAME.png"
done

if [ -f "$DESKTOP_FILE_SRC" ]; then
    cp "$DESKTOP_FILE_SRC" "$DESKTOP_DIR/"
    echo "Installed desktop file: $DESKTOP_DIR/$APP_NAME.desktop"
else
    echo "Warning: Desktop file $DESKTOP_FILE_SRC not found. Skipping."
fi

gtk-update-icon-cache "$ICON_THEME_DIR" || true
kbuildsycoca5 || true

echo "Done. You may need to log out and back in for changes to take effect."
