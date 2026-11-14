#!/bin/bash
# Build the .deb inside an Ubuntu container (used by CI, works locally too):
#   docker run --rm -v "$PWD:/src" -w /src ubuntu:26.04 packaging/build-deb.sh [version]
# CI passes the version from the tag; local default is 0.0.0
# Output: out/kcm-cloud-mouse_<ver>-1_amd64.deb
# Runtime library deps come from dpkg-shlibdeps; QML modules (not linked) are listed below.
set -euo pipefail
VER=${1:-0.0.0}
SRC=$(cd "$(dirname "$0")/.." && pwd)
PKG=kcm-cloud-mouse

export DEBIAN_FRONTEND=noninteractive
apt-get update -q
apt-get install -y -q --no-install-recommends \
    cmake g++ make extra-cmake-modules dpkg-dev file \
    qt6-base-dev qt6-declarative-dev \
    libkf6kcmutils-dev libkf6i18n-dev libkf6coreaddons-dev

build=$(mktemp -d)
root=$(mktemp -d); chmod 755 "$root"
cmake -S "$SRC" -B "$build" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TESTING=OFF
cmake --build "$build" -j"$(nproc)"
DESTDIR="$root" cmake --install "$build" --strip
install -Dm644 "$SRC/README.md" "$root/usr/share/doc/$PKG/README.md"

# dpkg-shlibdeps wants a debian/control next to it
work=$(mktemp -d)
mkdir -p "$work/debian"
printf 'Source: %s\n\nPackage: %s\nArchitecture: any\n' "$PKG" "$PKG" > "$work/debian/control"
mapfile -t libs < <(find "$root" -type f -name '*.so*')
shlibs=$(cd "$work" && dpkg-shlibdeps -O "${libs[@]}" | sed -n 's/^shlibs:Depends=//p')

ARCH=$(dpkg --print-architecture)
mkdir -p "$root/DEBIAN"
cat > "$root/DEBIAN/control" <<EOF
Package: $PKG
Version: $VER-1
Architecture: $ARCH
Maintainer: chiririll <noreply@github.com>
Section: kde
Priority: optional
Installed-Size: $(du -sk --exclude=DEBIAN "$root" | cut -f1)
Depends: $shlibs, qml6-module-org-kde-kcmutils, qml6-module-org-kde-kirigami, qml6-module-qtquick, qml6-module-qtquick-controls, qml6-module-qtquick-layouts
Homepage: https://github.com/chiririll/kcm-cloud-mouse
Description: KDE System Settings module for mouse settings over org.gamescope.Input
 Cloud Mouse: pointer speed, acceleration profile, scrolling and buttons for
 input devices handled by a compositor that exposes the org.gamescope.Input
 D-Bus interface (Gamescope Cloud Desktop, cloud-desktop-host). Appears in
 System Settings -> Input Devices -> Cloud Mouse.
EOF

mkdir -p "$SRC/out"
out="$SRC/out/${PKG}_${VER}-1_${ARCH}.deb"
dpkg-deb --root-owner-group --build "$root" "$out"
dpkg-deb -I "$out"
dpkg-deb -c "$out"
