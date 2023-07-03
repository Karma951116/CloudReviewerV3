#ifndef DECODEWORKER_H
#define DECODEWORKER_H

#include <QObject>
#include <QThread>
#include <QVariant>

#include <hlsindex.h>

extern "C"
{
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/pixfmt.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}
struct VideoMeta {
    int width;
    int height;
    double frameRate;
    AVPixelFormat pixelFormat;
    double vTimeBase;
    AVRational vTimeBaseRational;
};
Q_DECLARE_METATYPE(VideoMeta)
struct AudioMeta {
    unsigned long long channelLayout;
    int sampleRate;
    AVSampleFormat sampleFormat;
    int channels;
    int samplePerFrame;
    double aTimeBase;
    AVRational aTimeBaseRational;
};
Q_DECLARE_METATYPE(AudioMeta)

class DecodeWorker : public QThread
{
    Q_OBJECT
public:
    struct buffer_data {
        uint8_t* data;
        int size;
        int offset;
    };

    DecodeWorker(HlsIndex* hlsIndex, int blockIndex);
    void run() override;
    static int read_packet(void *opaque, uint8_t *buf, int buf_size);
signals:
    void decodeFinished(QVector<AVFrame*>* vFrameVector, QVector<AVFrame*>* aFrameVector,
                        int blockIndex, QVariant vMeta, QVariant aMeta);
private:
    HlsIndex* hlsIndex_;
    int blockIndex_;
};

#endif // DECODEWORKER_H
