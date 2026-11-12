# Built by packaging/build-rpm.sh (CI: .github/workflows/release.yml):
#   rpmbuild -bb --define "pkgver 1.0.1" packaging/kcm-cloud-mouse.spec
# Source0 is a tarball of the repo, top dir kcm-cloud-mouse-%{pkgver}.
%{!?pkgver: %global pkgver 0.0.0}
# single small plugin: no -debuginfo/-debugsource subpackages
%global debug_package %{nil}

Name:           kcm-cloud-mouse
Version:        %{pkgver}
Release:        1%{?dist}
Summary:        KDE System Settings module for mouse settings over org.gamescope.Input
License:        GPL-2.0-or-later
URL:            https://github.com/chiririll/kcm-cloud-mouse
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  extra-cmake-modules
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  kf6-kcmutils-devel
BuildRequires:  kf6-ki18n-devel
BuildRequires:  kf6-kcoreaddons-devel

Requires:       kf6-kcmutils
Requires:       kf6-kirigami

%description
Cloud Mouse: pointer speed, acceleration profile, scrolling and buttons for
input devices handled by a compositor that exposes the org.gamescope.Input
D-Bus interface (Gamescope Cloud Desktop, cloud-desktop-host). Appears in
System Settings -> Input Devices -> Cloud Mouse.

%prep
%autosetup

%build
%cmake -DCMAKE_BUILD_TYPE=Release
%cmake_build

%install
%cmake_install

%files
%doc README.md
%{_libdir}/qt6/plugins/plasma/kcms/systemsettings/kcm_cloud_mouse.so
%{_datadir}/applications/kcm_cloud_mouse.desktop
