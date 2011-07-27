#ifndef QMLPIXMAP_H
#define QMLPIXMAP_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtGui/QPixmap>


class QmlPixmap : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)

public:
    explicit QmlPixmap(QDeclarativeItem *parent = 0);

    QPixmap pixmap() const;
    void setPixmap(QPixmap const& p);

signals:
    void pixmapChanged();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap m_pixmap;
};

QML_DECLARE_TYPE(QmlPixmap)
QML_DECLARE_TYPE(QPixmap)

#endif // QMLPIXMAP_H
