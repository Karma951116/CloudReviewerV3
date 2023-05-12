#include "framequeue.h"
#include <QDebug>
#include <QThread>


FrameQueue::FrameQueue()
{
    firstEnqueueFlag = false;
    //rIndex = 0;
//    qDebug() << QString("QueueThread:") +
//                QString::number(quintptr(QThread::currentThreadId()));
}

FrameQueue::~FrameQueue()
{
    qDebug() << "FrameQueue析构";
    emptyAll();
}

void FrameQueue::enqueue(AVFrame* frame)
{
    QWriteLocker locker(&lock);
    queue.enqueue(frame);
//    if (queueSize() == 1) {
//        emit firstItemEnqueue();
//    }
}

AVFrame* FrameQueue::dequeue()
{
    if (queue.isEmpty()) {
        return  nullptr;
    }
    else {
        QWriteLocker locker(&lock);
        AVFrame* tmp = queue.dequeue();
        return tmp;
    }
}

void FrameQueue::emptyAll()
{
    while (queue.size() > 0) {
        AVFrame* frame = dequeue();
        av_frame_free(&frame);
    }
}

bool FrameQueue::isEmpty()
{
    return queue.isEmpty();             //返回队列是否为空
}

int FrameQueue::queueSize()
{
    QReadLocker locker(&lock);
    int size = queue.size();
    return size;
}

AVFrame *FrameQueue::read(int index)
{
    if ((index > queue.size() - 1) || index < 0) {
        return nullptr;
    }
    QReadLocker locker(&lock);
    AVFrame* frame = queue.at(index);
    return frame;
}
