#ifndef PAINTCANVAS_H
#define PAINTCANVAS_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QLineF>
#include <QPen>
#include <QVector>
#include <QPainter>
#include <QStack>

class PaintCanvas : public QQuickPaintedItem
{
    Q_OBJECT
public:
    PaintCanvas();

    enum PaintMode {
        PEN,
        RECT,
        CIRCLE,
        ARROW
    };
    Q_ENUM(PaintMode)

    struct DrawsGroup {
        QVector<QLineF> lines;
        QRectF rect;
        QPen pen;
        PaintMode mode;
    };

    Q_INVOKABLE void clear();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void setPaintMode(PaintMode mode);
    Q_INVOKABLE void setImage(QImage target_image);
    Q_INVOKABLE QImage getImage();
    Q_INVOKABLE bool hasDraws();
    //Q_INVOKABLE bool save();

    //float getPenWidth() const;
    Q_INVOKABLE void setPenWidth(float pen_width);
    //QColor getPaintColor() const;
    Q_INVOKABLE void setPaintColor(QColor paint_color);

    void paint(QPainter *painter);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void savePaint();

    bool pressed_;
    bool moved_;
    bool output_;
    // 0: pen 1: rect 2: circle 3:arrow
    PaintMode paint_mode_;

    QPixmap target_image_;
    QPixmap backup_image_;
    QPainter image_painter_;
    QPen current_pen_;
    QPointF last_point_;
    DrawsGroup* current_draw_;
    QStack<DrawsGroup*> history_draws_;
    QStack<DrawsGroup*> redo_draws_;
};

#endif // PAINTCANVAS_H
