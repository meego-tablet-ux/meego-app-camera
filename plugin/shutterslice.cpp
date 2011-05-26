#include "shutterslice.h"
#include <QPen>
#include <QBrush>

ShutterSlice::ShutterSlice(qreal radius, qreal baseAngle, QGraphicsItem *item, QRectF d, QObject *objParent)
    : QObject(objParent)
    , m_baseAngle(baseAngle)
    , m_r(radius)
    , m_dd(d)
    , m_item(item)
    , m_angle(90)
{
}

qreal ShutterSlice::angle()
{
    return 0.0;
}

void ShutterSlice::setAngle(qreal angle)
{
    m_angle = angle;
    setRotation();
}

void ShutterSlice::setRect(QRectF rect)
{
    m_dd = rect;
}

void ShutterSlice::setRotation()
{
    m_item->setTransform(QTransform().translate(m_dd.width()/2,m_dd.height()/2).rotate(m_baseAngle).translate(0,m_r).rotate(m_angle).translate(0,-m_r));
}
