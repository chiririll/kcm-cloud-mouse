/*
    Cloud Mouse QML UI — настройка мыши
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCMUtils

KCMUtils.SimpleKCM {
    id: root

    property var backend: kcm.backend
    property var devices: backend ? backend.devices : []

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: devices.length === 0
        icon.name: "input-mouse"
        text: backend && backend.errorString !== ""
              ? backend.errorString
              : i18n("No mouse devices found")
        explanation: i18n("Make sure Gamescope is running with D-Bus support enabled")
    }

    Kirigami.FormLayout {
        visible: devices.length > 0

        Repeater {
            model: devices
            delegate: deviceDelegate
        }
    }

    Component {
        id: deviceDelegate

        ColumnLayout {
            required property var modelData
            property var dev: modelData

            Kirigami.FormLayout {
                // Device name header
                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: dev.name
                }

                // Pointer acceleration
                QQC2.Slider {
                    visible: dev.supportsPointerAcceleration
                    Kirigami.FormData.label: i18n("Pointer speed:")
                    from: -1.0
                    to: 1.0
                    stepSize: 0.01
                    value: dev.pointerAcceleration
                    onMoved: dev.pointerAcceleration = value
                }

                QQC2.Label {
                    visible: dev.supportsPointerAcceleration
                    text: (dev.pointerAcceleration >= 0 ? "+" : "") + dev.pointerAcceleration.toFixed(2)
                    Layout.alignment: Qt.AlignHCenter
                }

                // Acceleration profile
                QQC2.ButtonGroup {
                    id: accelProfileGroup
                }

                QQC2.RadioButton {
                    visible: dev.supportsPointerAccelerationProfileAdaptive
                    Kirigami.FormData.label: i18n("Acceleration profile:")
                    text: i18n("Adaptive")
                    checked: dev.pointerAccelerationProfileAdaptive
                    QQC2.ButtonGroup.group: accelProfileGroup
                    onToggled: {
                        dev.pointerAccelerationProfileAdaptive = checked
                        dev.pointerAccelerationProfileFlat = !checked
                    }
                }

                QQC2.RadioButton {
                    visible: dev.supportsPointerAccelerationProfileFlat
                    text: i18n("Flat")
                    checked: dev.pointerAccelerationProfileFlat
                    QQC2.ButtonGroup.group: accelProfileGroup
                    onToggled: {
                        dev.pointerAccelerationProfileFlat = checked
                        dev.pointerAccelerationProfileAdaptive = !checked
                    }
                }

                // Natural scroll
                QQC2.CheckBox {
                    visible: dev.supportsNaturalScroll
                    Kirigami.FormData.label: i18n("Scrolling:")
                    text: i18n("Invert scroll direction (Natural scrolling)")
                    checked: dev.naturalScroll
                    onToggled: dev.naturalScroll = checked
                }

                // Scroll factor
                QQC2.Slider {
                    Kirigami.FormData.label: i18n("Scroll speed:")
                    from: 0.1
                    to: 16.0
                    stepSize: 0.1
                    value: dev.scrollFactor
                    onMoved: dev.scrollFactor = value
                }

                QQC2.Label {
                    text: "×" + dev.scrollFactor.toFixed(1)
                    Layout.alignment: Qt.AlignHCenter
                }

                // Left-handed
                QQC2.CheckBox {
                    visible: dev.supportsLeftHanded
                    Kirigami.FormData.label: i18n("Buttons:")
                    text: i18n("Left handed mode")
                    checked: dev.leftHanded
                    onToggled: dev.leftHanded = checked
                }

                // Middle emulation
                QQC2.CheckBox {
                    visible: dev.supportsMiddleEmulation
                    text: i18n("Press left and right buttons for middle-click")
                    checked: dev.middleEmulation
                    onToggled: dev.middleEmulation = checked
                }
            }
        }
    }
}
