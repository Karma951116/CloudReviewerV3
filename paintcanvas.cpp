#include "paintcanvas.h"

PaintCanvas::PaintCanvas() :
    pressed_(false),
    moved_(false),
    output_(false),
    paint_mode_(PEN),
    current_pen_(Qt::black),
    current_draw_(nullptr)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

void PaintCanvas::clear()
{
    for(DrawsGroup* item : history_draws_) {
        delete item;
    }
    history_draws_.clear();
    for(DrawsGroup* item : redo_draws_) {
        delete item;
    }
    redo_draws_.clear();
    // current_draw_ 从单击左键开始已进入history栈，无需再次delete
    current_draw_ = nullptr;
    update();
}

void PaintCanvas::undo()
{
    if (!history_draws_.empty()) {
        redo_draws_.push(history_draws_.pop());
        update();
    }
}

void PaintCanvas::redo()
{
    if (!redo_draws_.empty()) {
        history_draws_.push(redo_draws_.pop());
        update();
    }
}

void PaintCanvas::setPaintMode(PaintCanvas::PaintMode mode)
{
    paint_mode_ = mode;
}

void PaintCanvas::setImage(QImage target_image)
{
    int width = this->width();
    int height = this->height();
    target_image_.convertFromImage(target_image);
    target_image_ = target_image_.scaled(QSize(width, height), Qt::KeepAspectRatio);
    backup_image_ = target_image_;
}

QImage PaintCanvas::getImage()
{
    savePaint();
    //target_image_.save("E:\\test.png");
    return target_image_.toImage();
}

bool PaintCanvas::hasDraws()
{
    return !history_draws_.isEmpty();
}

void PaintCanvas::setPenWidth(float pen_width)
{
    current_pen_.setWidthF(pen_width);
}

void PaintCanvas::setPaintColor(QColor paint_color)
{
    current_pen_.setColor(paint_color);
}

void PaintCanvas::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    if (!current_draw_ || history_draws_.empty()) {
        return;
    }

    for (DrawsGroup* item : history_draws_) {
        painter->setPen(item->pen);
        switch (item->mode) {
        case PEN:
        case ARROW:
            if (!item->lines.empty()) {
                painter->drawLines(item->lines);
            }
            break;
        case RECT:
            if(!item->rect.isNull()) {
                painter->drawRect(item->rect);
            }
            break;
        case CIRCLE:
            if(!item->rect.isNull()) {
                painter->drawEllipse(item->rect);
            }
            break;
        }
    }
}

void PaintCanvas::mousePressEvent(QMouseEvent *event)
{
    if(!(event->button() & acceptedMouseButtons()))
    {
        QQuickPaintedItem::mousePressEvent(event);
        return;
    }
    pressed_ = true;
    current_draw_ = new DrawsGroup();
    current_draw_->pen = current_pen_;
    current_draw_->mode = paint_mode_;
    history_draws_.push(current_draw_);
    last_point_ = event->localPos();
    event->setAccepted(true);
}

void PaintCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!pressed_ || !current_draw_) {
        QQuickPaintedItem::mousePressEvent(event);
        return;
    }
    switch (paint_mode_) {
    case PEN:
        current_draw_->lines.append(QLineF(last_point_, event->localPos()));
        last_point_ = event->localPos();
        break;
    case RECT:
    // 圆和矩形都可通过rect绘制，可使用同一个变量存储
    case CIRCLE:
        current_draw_->rect = (QRectF(last_point_, event->localPos()));
        break;
    case ARROW:
        current_draw_->lines.clear();
        QPointF current_point = event->localPos();
        QLineF main_line = QLineF(last_point_, current_point);
        current_draw_->lines.append(main_line);

        QLineF unit_vector = main_line.unitVector();
        unit_vector.setLength(20);
        unit_vector.translate(QPointF(main_line.dx(), main_line.dy()));
        QLineF n1 = unit_vector.normalVector();
        n1.setLength(n1.length() * 0.5);
        QLineF n2 = n1.normalVector().normalVector();
        QPointF p1 = unit_vector.p2();
        QPointF p2 = n1.p2();
        QPointF p3 = n2.p2();
        current_draw_->lines.append(QLineF(p1, p2));
        current_draw_->lines.append(QLineF(p2, p3));
        current_draw_->lines.append(QLineF(p3, p1));
        break;
    }
    update();
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(!(event->button() & acceptedMouseButtons()))
    {
        QQuickPaintedItem::mousePressEvent(event);
        return;
    }
    pressed_ = false;
    switch (paint_mode_) {
    case PEN:
        current_draw_->lines.append(QLineF(last_point_, event->localPos()));
        break;
    case RECT:
    case CIRCLE:
        current_draw_->rect = (QRectF(last_point_, event->localPos()));
        break;
    case ARROW:
        break;
    }
    update();
}

void PaintCanvas::savePaint()
{
    if (target_image_.isNull()) {
        return;
    }
    image_painter_.begin(&target_image_);
    for (DrawsGroup* item : history_draws_) {
        image_painter_.setPen(item->pen);
        switch (item->mode) {
        case PEN:
        case ARROW:
            if (!item->lines.empty()) {
                image_painter_.drawLines(item->lines);
            }
            break;
        case RECT:
            if(!item->rect.isNull()) {
                image_painter_.drawRect(item->rect);
            }
            break;
        case CIRCLE:
            if(!item->rect.isNull()) {
                image_painter_.drawEllipse(item->rect);
            }
            break;
        }
    }
    image_painter_.end();
}
