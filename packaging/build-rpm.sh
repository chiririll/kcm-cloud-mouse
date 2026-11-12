#!/bin/bash
# Build the RPM inside a Fedora container (used by CI, works locally too):
#   docker run --rm -v "$PWD:/src" -w /src fedora:43 packaging/build-rpm.sh [version]
# CI passes the version from the tag; local default is 0.0.0
# Output: out/kcm-cloud-mouse-<ver>-1.fcNN.x86_64.rpm
set -euo pipefail
VER=${1:-0.0.0}
SRC=$(cd "$(dirname "$0")/.." && pwd)
SPEC=$SRC/packaging/kcm-cloud-mouse.spec

dnf install -y -q rpm-build 'dnf-command(builddep)'
dnf builddep -y -q "$SPEC"

top=$(mktemp -d)
mkdir -p "$top/SOURCES"
tar -C "$SRC" --exclude=./.git --exclude=./build --exclude=./out \
    --transform "s,^\.,kcm-cloud-mouse-$VER," -czf "$top/SOURCES/kcm-cloud-mouse-$VER.tar.gz" .
rpmbuild -bb --define "_topdir $top" --define "pkgver $VER" "$SPEC"

mkdir -p "$SRC/out"
cp "$top"/RPMS/*/*.rpm "$SRC/out/"
rpm -qlp "$SRC"/out/*.rpm
