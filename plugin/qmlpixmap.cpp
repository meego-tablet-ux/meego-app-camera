#include "qmlpixmap.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>


QmlPixmap::QmlPixmap(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
}

QPixmap QmlPixmap::pixmap() const
{
    return m_pixmap;
}

void QmlPixmap::setPixmap(QPixmap const& p)
{
    m_pixmap = p;
    update();

    emit pixmapChanged();
}

void QmlPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->drawPixmap(boundingRect().toRect(), m_pixmap);
    m_pixmap.save("/home/meego/tst.png");
}
