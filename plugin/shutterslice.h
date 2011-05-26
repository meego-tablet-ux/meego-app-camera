#ifndef SHUTTERSLICE_H
#define SHUTTERSLICE_H

#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>

class ShutterSlice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
public:
    ShutterSlice(qreal radius, qreal baseAngle, QGraphicsItem *item, QRectF d, QObject *objParent=0);

    void setAngle(qreal angle);

    void setRect(QRectF rect);

protected:
    void setRotation();
    qreal m_baseAngle;
    qreal m_r; //radius
    QRectF m_dd;
    QGraphicsItem *m_item;
    qreal m_angle;

private:
    qreal angle();

};

#endif // SHUTTERSLICE_H
