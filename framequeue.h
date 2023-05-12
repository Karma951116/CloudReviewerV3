#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include <QQueue>
#include <QObject>
#include <QReadWriteLock>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

#include "SDL.h"

class FrameQueue : public QObject
{
    Q_OBJECT
public:
    FrameQueue();
    ~FrameQueue();

    void enqueue(AVFrame* frame);                     // 在队列尾部添加一个元素
    AVFrame* dequeue();       // 删除当前队列第一个元素,并返回这个元素
    bool isEmpty();                                     // 返回队列是否为空
    void emptyAll();
    int queueSize();
    AVFrame* read(int index);

//    SDL_mutex *getMutex() const;

//    SDL_cond *getCond() const;

signals:
    void firstItemEnqueue();
private:
//    SDL_mutex* mutex;
//    SDL_cond* cond;
    QReadWriteLock lock;
    bool firstEnqueueFlag;
    QQueue<AVFrame*> queue;
};

#endif // FRAMEQUEUE_H
