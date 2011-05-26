#include "shutteranimationcomponent.h"

#include <QtDeclarative/qdeclarative.h>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include "shutterslice.h"
#include <QDebug>
#include <QTimeLine>
#include <QtCore/qmath.h>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>


ShutterAnimationComponent::ShutterAnimationComponent(QDeclarativeItem *parent):
        QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:

    // setFlag(ItemHasNoContents, false);
    setVisible(false);
}

ShutterAnimationComponent::~ShutterAnimationComponent()
{
}

qreal ShutterAnimationComponent::size()
{
    return m_size;
}

void ShutterAnimationComponent::setSize(qreal size)
{
    m_size = size;
    emit sizeChanged();
}

void ShutterAnimationComponent::componentComplete()
{
    QGraphicsItem *base = this;
    base->setFlag(QGraphicsItem::ItemClipsChildrenToShape,true);

    qreal d = qMax(base->boundingRect().width(),base->boundingRect().height());
    qDebug() << d;
    qreal r = d/2.0;
    r *= 1.4;

    qreal incrementAngle = 36;


    qreal b = r;
    QPolygonF poly;
    poly << QPointF(0,0) << QPointF(0,r) << QPointF(b,r) << QPointF(0,0);


    int i=0;
    for (qreal angle = 0.0; angle < 360; angle += incrementAngle, i++) {

        // Could be QRectF
        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly,this);
        polyItem->setPen(QPen(Qt::black));
        polyItem->setBrush(QColor("gray"));
        //polyItem->setTransform(QTransform().translate(width()/2,height()/2).rotate(angle).translate(0,r).rotate(80).translate(0,-r));

       ShutterSlice *sh= new ShutterSlice(r,angle,polyItem,boundingRect());
       shList.append(sh);

        QPropertyAnimation *anim = new QPropertyAnimation(sh,"angle");
        anim->setDuration(3000);
        anim->setKeyValueAt(0,90.0);
        anim->setKeyValueAt(0.2, 0.0);
        anim->setKeyValueAt(0.8, 0.0);
        anim->setKeyValueAt(1,90.0);
        animationGroup.addAnimation(anim);
    }


}

void ShutterAnimationComponent::start()
{
    setVisible(true);
    animationGroup.start();
}

void ShutterAnimationComponent::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_UNUSED(oldGeometry);
    foreach(ShutterSlice* slice, shList)
    {
        slice->setRect(newGeometry);
    }
}

void ShutterAnimationComponent::animationEnded()
{
    setVisible(false);
}