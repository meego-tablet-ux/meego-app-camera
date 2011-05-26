#ifndef SHUTTERANIMATIONCOMPONENT_H
#define SHUTTERANIMATIONCOMPONENT_H

#include <QtDeclarative/QDeclarativeItem>
#include <QParallelAnimationGroup>
#include "shutterslice.h"

class ShutterAnimationComponent : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ShutterAnimationComponent)

    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)

public:
    ShutterAnimationComponent(QDeclarativeItem *parent = 0);
    ~ShutterAnimationComponent();

    qreal size();
    void setSize(qreal size);

    virtual void componentComplete();

    Q_INVOKABLE void start();

signals:
    void sizeChanged();

protected:
   void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

protected slots:
   void animationEnded();


private:
    qreal m_size;
    QParallelAnimationGroup animationGroup;
    QList<ShutterSlice*> shList;
};

QML_DECLARE_TYPE(ShutterAnimationComponent)

#endif // SHUTTERANIMATIONCOMPONENT_H

