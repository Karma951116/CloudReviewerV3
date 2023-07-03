#ifndef TSROLL_H
#define TSROLL_H

#include <QObject>
#include <QReadWriteLock>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

class TsRoll : public QObject
{
    Q_OBJECT
public:
    explicit TsRoll(int blockCount);
    void setTsBlock(int index, QVector<AVFrame*>* block);
    bool isBlockEmpty(int blockIdx);
    bool isEmpty();
    int blockSize(int blockIdx);
    int size();
    void emptyBlock(int blockIdx);
    void empty();
    AVFrame* read(int blockIdx, int frameIdx);
    AVFrame* read();
    void nextFrame();
    void previousFrame();
    void nextBlock();
    void previousBlock();
    bool isFirstFrame();
    bool isLastFrame();
    bool isFirstBlock();
    bool isLastBlock();
    bool seek(int blockIdx, int frameIdx);
    bool hasBlock(int blockIdx);
    bool canRead(int blockIdx, int frameIdx);
    bool canRead();

    int tsBlockIndex() const;

    int tsFrameIndex() const;

private:
    int tsBlockIndex_;
    int tsFrameIndex_;
    QReadWriteLock lock;
    QVector<QVector<AVFrame*>*> roll;

signals:
    void clearBuffer();
    void blockCleared(int blockIndex);

private slots:
    void onClearBuffer();
};

#endif // TSROLL_H
