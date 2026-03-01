/*
    Cloud Mouse Device — D-Bus proxy к org.gamescope.Input
    Читает/пишет properties libinput устройства через Gamescope D-Bus.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QString>
#include <QDBusInterface>

class CloudMouseDevice : public QObject
{
    Q_OBJECT

    // General
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString sysName READ sysName CONSTANT)
    Q_PROPERTY(bool supportsDisableEvents READ supportsDisableEvents CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY needsSaveChanged)

    // Acceleration
    Q_PROPERTY(bool supportsPointerAcceleration READ supportsPointerAcceleration CONSTANT)
    Q_PROPERTY(qreal pointerAcceleration READ pointerAcceleration WRITE setPointerAcceleration NOTIFY needsSaveChanged)
    Q_PROPERTY(qreal defaultPointerAcceleration READ defaultPointerAcceleration CONSTANT)

    Q_PROPERTY(bool supportsPointerAccelerationProfileFlat READ supportsPointerAccelerationProfileFlat CONSTANT)
    Q_PROPERTY(bool defaultPointerAccelerationProfileFlat READ defaultPointerAccelerationProfileFlat CONSTANT)
    Q_PROPERTY(bool pointerAccelerationProfileFlat READ pointerAccelerationProfileFlat WRITE setPointerAccelerationProfileFlat NOTIFY needsSaveChanged)

    Q_PROPERTY(bool supportsPointerAccelerationProfileAdaptive READ supportsPointerAccelerationProfileAdaptive CONSTANT)
    Q_PROPERTY(bool defaultPointerAccelerationProfileAdaptive READ defaultPointerAccelerationProfileAdaptive CONSTANT)
    Q_PROPERTY(bool pointerAccelerationProfileAdaptive READ pointerAccelerationProfileAdaptive WRITE setPointerAccelerationProfileAdaptive NOTIFY needsSaveChanged)

    // Natural scroll
    Q_PROPERTY(bool supportsNaturalScroll READ supportsNaturalScroll CONSTANT)
    Q_PROPERTY(bool naturalScrollEnabledByDefault READ naturalScrollEnabledByDefault CONSTANT)
    Q_PROPERTY(bool naturalScroll READ isNaturalScroll WRITE setNaturalScroll NOTIFY needsSaveChanged)

    // Scroll factor
    Q_PROPERTY(qreal scrollFactor READ scrollFactor WRITE setScrollFactor NOTIFY needsSaveChanged)

    // Left-handed
    Q_PROPERTY(bool supportsLeftHanded READ supportsLeftHanded CONSTANT)
    Q_PROPERTY(bool leftHandedEnabledByDefault READ leftHandedEnabledByDefault CONSTANT)
    Q_PROPERTY(bool leftHanded READ isLeftHanded WRITE setLeftHanded NOTIFY needsSaveChanged)

    // Middle emulation
    Q_PROPERTY(bool supportsMiddleEmulation READ supportsMiddleEmulation CONSTANT)
    Q_PROPERTY(bool middleEmulationEnabledByDefault READ middleEmulationEnabledByDefault CONSTANT)
    Q_PROPERTY(bool middleEmulation READ isMiddleEmulation WRITE setMiddleEmulation NOTIFY needsSaveChanged)

public:
    explicit CloudMouseDevice(const QString &sysName, QObject *parent = nullptr);

    bool init();
    bool save();
    bool defaults();
    bool isSaveNeeded() const;

    // General
    QString name() const { return m_name; }
    QString sysName() const { return m_sysName; }
    bool supportsDisableEvents() const { return m_supportsDisableEvents; }
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool set);

    // Acceleration
    bool supportsPointerAcceleration() const { return m_supportsPointerAcceleration; }
    qreal pointerAcceleration() const { return m_pointerAcceleration; }
    qreal defaultPointerAcceleration() const { return m_defaultPointerAcceleration; }
    void setPointerAcceleration(qreal accel);

    bool supportsPointerAccelerationProfileFlat() const { return m_supportsAccelProfileFlat; }
    bool defaultPointerAccelerationProfileFlat() const { return m_defaultAccelProfileFlat; }
    bool pointerAccelerationProfileFlat() const { return m_accelProfileFlat; }
    void setPointerAccelerationProfileFlat(bool set);

    bool supportsPointerAccelerationProfileAdaptive() const { return m_supportsAccelProfileAdaptive; }
    bool defaultPointerAccelerationProfileAdaptive() const { return m_defaultAccelProfileAdaptive; }
    bool pointerAccelerationProfileAdaptive() const { return m_accelProfileAdaptive; }
    void setPointerAccelerationProfileAdaptive(bool set);

    // Natural scroll
    bool supportsNaturalScroll() const { return m_supportsNaturalScroll; }
    bool naturalScrollEnabledByDefault() const { return m_naturalScrollDefault; }
    bool isNaturalScroll() const { return m_naturalScroll; }
    void setNaturalScroll(bool set);

    // Scroll factor
    qreal scrollFactor() const { return m_scrollFactor; }
    void setScrollFactor(qreal factor);

    // Left-handed
    bool supportsLeftHanded() const { return m_supportsLeftHanded; }
    bool leftHandedEnabledByDefault() const { return m_leftHandedDefault; }
    bool isLeftHanded() const { return m_leftHanded; }
    void setLeftHanded(bool set);

    // Middle emulation
    bool supportsMiddleEmulation() const { return m_supportsMiddleEmulation; }
    bool middleEmulationEnabledByDefault() const { return m_middleEmulationDefault; }
    bool isMiddleEmulation() const { return m_middleEmulation; }
    void setMiddleEmulation(bool set);

Q_SIGNALS:
    void needsSaveChanged();

private:
    QVariant getProperty(const QString &name) const;
    bool setDBusProperty(const QString &name, const QVariant &value);

    static constexpr const char *s_serviceName = "org.gamescope.Input";
    static constexpr const char *s_iface = "org.gamescope.InputDevice";

    QString m_sysName;
    QString m_dbusPath;
    QDBusInterface *m_iface = nullptr;

    // Cached values (current)
    QString m_name;
    bool m_supportsDisableEvents = false;
    bool m_enabled = true;
    bool m_supportsPointerAcceleration = false;
    qreal m_pointerAcceleration = 0.0;
    qreal m_defaultPointerAcceleration = 0.0;
    bool m_supportsAccelProfileFlat = false;
    bool m_defaultAccelProfileFlat = false;
    bool m_accelProfileFlat = false;
    bool m_supportsAccelProfileAdaptive = false;
    bool m_defaultAccelProfileAdaptive = false;
    bool m_accelProfileAdaptive = false;
    bool m_supportsNaturalScroll = false;
    bool m_naturalScrollDefault = false;
    bool m_naturalScroll = false;
    qreal m_scrollFactor = 1.0;
    bool m_supportsLeftHanded = false;
    bool m_leftHandedDefault = false;
    bool m_leftHanded = false;
    bool m_supportsMiddleEmulation = false;
    bool m_middleEmulationDefault = false;
    bool m_middleEmulation = false;

    // Original values (for save tracking)
    bool m_origEnabled = true;
    qreal m_origPointerAcceleration = 0.0;
    bool m_origAccelProfileFlat = false;
    bool m_origAccelProfileAdaptive = false;
    bool m_origNaturalScroll = false;
    qreal m_origScrollFactor = 1.0;
    bool m_origLeftHanded = false;
    bool m_origMiddleEmulation = false;
};
