#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QReadWriteLock>

#include "hlsindex.h"
#include "framequeue.h"

extern "C"
{
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/pixfmt.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libavutil/imgutils.h"
}

class VideoPlayer : public QQuickPaintedItem
{
    Q_OBJECT
public:
    enum PlayState {
        PLAY,
        PAUSE,
        STOP
    };
    Q_ENUM(PlayState);
    VideoPlayer();

    HlsIndex *getHlsIndex() const;
    void setHlsIndex(HlsIndex *index);

    int getFetchIndex() const;
    void setFetchIndex(int fetchIndex);

    PlayState getState() const;
    void setState(const PlayState &state);

    int getDecodeIndex() const;
    void setDecodeIndex(int decodeIndex);
    static int read_packet(void *opaque, uint8_t *buf, int buf_size);
    FrameQueue* videoQueue_;
    FrameQueue* audioQueue_;

    bool getHasVideo() const;
    void setHasVideo(bool value);

    bool getHasAudio() const;
    void setHasAudio(bool value);

protected:
    virtual void paint(QPainter* painter);

private:
    QImage image_;
    PlayState state_;

    HlsIndex* index_;
    int maxDecoderCount_;
    int tsDecoderCount_;
    QReadWriteLock fetchLock_;
    QReadWriteLock decodeLock_;

    SwsContext* swsContext_;
    unsigned char* swsOutBuffer_;
    bool hasVideo;
    bool hasAudio;

    SDL_Thread* tsFetcher_;
    SDL_Thread* tsDecoder_;
    SDL_Thread* videoThread_;

    int fetchIndex_;
    static int tsFetch(void *arg);
    int decodeIndex_;
    static int tsDecode(void *arg);

    static int videoPlay(void *arg);
    QMap<int, QPair<int, int>> timeMap;

signals:
    void requestTsFile(QString tsName, TsFile* TS, QString url = nullptr);
    void requestMediaInfo(QString tsName, TsFile* TS, QString url = nullptr);
public slots:
    void onM3u8ReplyDone(bool success, HlsIndex* index);
    void onMediaInfoReplyDone(bool success, int nbFrames, int frameRate);
private slots:
    void onUpdateFrame(QImage image);
};

#endif // VIDEOPLAYER_H
