/*
    Cloud Mouse Device — D-Bus proxy реализация
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cloud_mouse_device.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDebug>

CloudMouseDevice::CloudMouseDevice(const QString &sysName, QObject *parent)
    : QObject(parent)
    , m_sysName(sysName)
    , m_dbusPath(QStringLiteral("/org/gamescope/Input/Device/") + sysName)
{
}

QVariant CloudMouseDevice::getProperty(const QString &name) const
{
    if (!m_iface)
        return QVariant();
    return m_iface->property(name.toLatin1().constData());
}

bool CloudMouseDevice::setDBusProperty(const QString &name, const QVariant &value)
{
    // Используем org.freedesktop.DBus.Properties.Set для записи D-Bus property
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QLatin1String(s_serviceName),
        m_dbusPath,
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("Set"));

    msg << QString::fromLatin1(s_iface) << name << QVariant::fromValue(QDBusVariant(value));

    QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "[cloud-mouse] Set" << name << "failed:" << reply.errorMessage();
        return false;
    }
    qDebug() << "[cloud-mouse] Set" << m_sysName << name << "=" << value;
    return true;
}

bool CloudMouseDevice::init()
{
    m_iface = new QDBusInterface(
        QLatin1String(s_serviceName),
        m_dbusPath,
        QLatin1String(s_iface),
        QDBusConnection::sessionBus(),
        this);

    if (!m_iface->isValid()) {
        qWarning() << "[cloud-mouse] Invalid D-Bus interface for" << m_sysName;
        return false;
    }

    // Read all properties
    m_name = getProperty(QStringLiteral("name")).toString();
    m_supportsDisableEvents = getProperty(QStringLiteral("supportsDisableEvents")).toBool();
    m_enabled = getProperty(QStringLiteral("enabled")).toBool();

    m_supportsPointerAcceleration = getProperty(QStringLiteral("supportsPointerAcceleration")).toBool();
    m_pointerAcceleration = getProperty(QStringLiteral("pointerAcceleration")).toDouble();
    m_defaultPointerAcceleration = getProperty(QStringLiteral("defaultPointerAcceleration")).toDouble();

    m_supportsAccelProfileFlat = getProperty(QStringLiteral("supportsPointerAccelerationProfileFlat")).toBool();
    m_defaultAccelProfileFlat = getProperty(QStringLiteral("defaultPointerAccelerationProfileFlat")).toBool();
    m_accelProfileFlat = getProperty(QStringLiteral("pointerAccelerationProfileFlat")).toBool();

    m_supportsAccelProfileAdaptive = getProperty(QStringLiteral("supportsPointerAccelerationProfileAdaptive")).toBool();
    m_defaultAccelProfileAdaptive = getProperty(QStringLiteral("defaultPointerAccelerationProfileAdaptive")).toBool();
    m_accelProfileAdaptive = getProperty(QStringLiteral("pointerAccelerationProfileAdaptive")).toBool();

    m_supportsNaturalScroll = getProperty(QStringLiteral("supportsNaturalScroll")).toBool();
    m_naturalScrollDefault = getProperty(QStringLiteral("naturalScrollEnabledByDefault")).toBool();
    m_naturalScroll = getProperty(QStringLiteral("naturalScroll")).toBool();

    m_scrollFactor = getProperty(QStringLiteral("scrollFactor")).toDouble();
    if (m_scrollFactor <= 0.0) m_scrollFactor = 1.0;

    m_supportsLeftHanded = getProperty(QStringLiteral("supportsLeftHanded")).toBool();
    m_leftHandedDefault = getProperty(QStringLiteral("leftHandedEnabledByDefault")).toBool();
    m_leftHanded = getProperty(QStringLiteral("leftHanded")).toBool();

    m_supportsMiddleEmulation = getProperty(QStringLiteral("supportsMiddleEmulation")).toBool();
    m_middleEmulationDefault = getProperty(QStringLiteral("middleEmulationEnabledByDefault")).toBool();
    m_middleEmulation = getProperty(QStringLiteral("middleEmulation")).toBool();

    // Save originals
    m_origEnabled = m_enabled;
    m_origPointerAcceleration = m_pointerAcceleration;
    m_origAccelProfileFlat = m_accelProfileFlat;
    m_origAccelProfileAdaptive = m_accelProfileAdaptive;
    m_origNaturalScroll = m_naturalScroll;
    m_origScrollFactor = m_scrollFactor;
    m_origLeftHanded = m_leftHanded;
    m_origMiddleEmulation = m_middleEmulation;

    qDebug() << "[cloud-mouse] Device loaded:" << m_name << "(" << m_sysName << ")";
    return true;
}

bool CloudMouseDevice::save()
{
    bool ok = true;

    if (m_enabled != m_origEnabled)
        ok &= setDBusProperty(QStringLiteral("enabled"), m_enabled);

    if (m_pointerAcceleration != m_origPointerAcceleration)
        ok &= setDBusProperty(QStringLiteral("pointerAcceleration"), m_pointerAcceleration);

    if (m_accelProfileFlat != m_origAccelProfileFlat)
        ok &= setDBusProperty(QStringLiteral("pointerAccelerationProfileFlat"), m_accelProfileFlat);

    if (m_accelProfileAdaptive != m_origAccelProfileAdaptive)
        ok &= setDBusProperty(QStringLiteral("pointerAccelerationProfileAdaptive"), m_accelProfileAdaptive);

    if (m_naturalScroll != m_origNaturalScroll)
        ok &= setDBusProperty(QStringLiteral("naturalScroll"), m_naturalScroll);

    if (m_scrollFactor != m_origScrollFactor)
        ok &= setDBusProperty(QStringLiteral("scrollFactor"), m_scrollFactor);

    if (m_leftHanded != m_origLeftHanded)
        ok &= setDBusProperty(QStringLiteral("leftHanded"), m_leftHanded);

    if (m_middleEmulation != m_origMiddleEmulation)
        ok &= setDBusProperty(QStringLiteral("middleEmulation"), m_middleEmulation);

    if (ok) {
        m_origEnabled = m_enabled;
        m_origPointerAcceleration = m_pointerAcceleration;
        m_origAccelProfileFlat = m_accelProfileFlat;
        m_origAccelProfileAdaptive = m_accelProfileAdaptive;
        m_origNaturalScroll = m_naturalScroll;
        m_origScrollFactor = m_scrollFactor;
        m_origLeftHanded = m_leftHanded;
        m_origMiddleEmulation = m_middleEmulation;
    }

    return ok;
}

bool CloudMouseDevice::defaults()
{
    if (m_supportsPointerAcceleration)
        setPointerAcceleration(m_defaultPointerAcceleration);
    if (m_supportsAccelProfileFlat)
        setPointerAccelerationProfileFlat(m_defaultAccelProfileFlat);
    if (m_supportsAccelProfileAdaptive)
        setPointerAccelerationProfileAdaptive(m_defaultAccelProfileAdaptive);
    if (m_supportsNaturalScroll)
        setNaturalScroll(m_naturalScrollDefault);
    setScrollFactor(1.0);
    if (m_supportsLeftHanded)
        setLeftHanded(m_leftHandedDefault);
    if (m_supportsMiddleEmulation)
        setMiddleEmulation(m_middleEmulationDefault);
    return true;
}

bool CloudMouseDevice::isSaveNeeded() const
{
    return m_enabled != m_origEnabled
        || m_pointerAcceleration != m_origPointerAcceleration
        || m_accelProfileFlat != m_origAccelProfileFlat
        || m_accelProfileAdaptive != m_origAccelProfileAdaptive
        || m_naturalScroll != m_origNaturalScroll
        || m_scrollFactor != m_origScrollFactor
        || m_leftHanded != m_origLeftHanded
        || m_middleEmulation != m_origMiddleEmulation;
}

void CloudMouseDevice::setEnabled(bool set) { if (m_enabled != set) { m_enabled = set; emit needsSaveChanged(); } }
void CloudMouseDevice::setPointerAcceleration(qreal accel) { if (m_pointerAcceleration != accel) { m_pointerAcceleration = accel; emit needsSaveChanged(); } }
void CloudMouseDevice::setPointerAccelerationProfileFlat(bool set) { if (m_accelProfileFlat != set) { m_accelProfileFlat = set; emit needsSaveChanged(); } }
void CloudMouseDevice::setPointerAccelerationProfileAdaptive(bool set) { if (m_accelProfileAdaptive != set) { m_accelProfileAdaptive = set; emit needsSaveChanged(); } }
void CloudMouseDevice::setNaturalScroll(bool set) { if (m_naturalScroll != set) { m_naturalScroll = set; emit needsSaveChanged(); } }
void CloudMouseDevice::setScrollFactor(qreal factor) { if (m_scrollFactor != factor) { m_scrollFactor = factor; emit needsSaveChanged(); } }
void CloudMouseDevice::setLeftHanded(bool set) { if (m_leftHanded != set) { m_leftHanded = set; emit needsSaveChanged(); } }
void CloudMouseDevice::setMiddleEmulation(bool set) { if (m_middleEmulation != set) { m_middleEmulation = set; emit needsSaveChanged(); } }

#include "moc_cloud_mouse_device.cpp"
