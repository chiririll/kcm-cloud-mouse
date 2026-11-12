cmake_minimum_required(VERSION 3.20)
# Version comes from the release tag (CI → packaging/build-rpm.sh), not from here
project(kcm_cloud_mouse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Qml Quick)
find_package(KF6 REQUIRED COMPONENTS KCMUtils I18n CoreAddons)

# KCM plugin
kcoreaddons_add_plugin(kcm_cloud_mouse SOURCES
    src/kcm_cloud_mouse.cpp
    src/cloud_mouse_backend.cpp
    src/cloud_mouse_device.cpp
    resources.qrc
    INSTALL_NAMESPACE "plasma/kcms/systemsettings"
)

target_link_libraries(kcm_cloud_mouse PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Qml
    Qt6::Quick
    KF6::KCMUtils
    KF6::I18n
    KF6::CoreAddons
)

# .desktop file
install(FILES kcm_cloud_mouse.desktop DESTINATION ${KDE_INSTALL_APPDIR})
