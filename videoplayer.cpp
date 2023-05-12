#include "videoplayer.h"


struct buffer_data {
    uint8_t* data;
    size_t size;
};

VideoPlayer::VideoPlayer()
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        qDebug() << "SDL init failed";
    }
}

HlsIndex *VideoPlayer::getHlsIndex() const
{
    return index_;
}

void VideoPlayer::setHlsIndex(HlsIndex *index)
{
    index_ = index;
}

int VideoPlayer::getFetchIndex() const
{
    return fetchIndex_;
}

void VideoPlayer::setFetchIndex(int fetchIndex)
{
    QWriteLocker locker(&fetchLock_);
    fetchIndex_ = fetchIndex;
}

VideoPlayer::PlayState VideoPlayer::getState() const
{
    return state_;
}

void VideoPlayer::setState(const PlayState &state)
{
    state_ = state;
}

int VideoPlayer::tsFetch(void *arg)
{
    VideoPlayer* player = (VideoPlayer*) arg;
    HlsIndex* index = player->getHlsIndex();
    QList<TsFile*> files = index->getFiles();
    player->setFetchIndex(0);
    while (!index->getFetchedAll() &&
           player->getState() != STOP) {
        int fetchIndex = player->getFetchIndex();
        if (!files[fetchIndex]->fetched()) {
            player->requestTsFile(files[fetchIndex]->fileName(), files[fetchIndex], nullptr);
        }
        fetchIndex += 1;
        if (fetchIndex >= files.size()) {
            fetchIndex = 0;
        }
        player->setFetchIndex(fetchIndex);
        SDL_Delay(200);
    }
    return 0;
}

int VideoPlayer::getDecodeIndex() const
{
    return decodeIndex_;
}

void VideoPlayer::setDecodeIndex(int decodeIndex)
{
    QWriteLocker locker(&decodeLock_);
    decodeIndex_ = decodeIndex;
}

int VideoPlayer::read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    buffer_data *bd = (buffer_data *) opaque;
    buf_size = FFMIN(buf_size, (int)bd->size);
    memcpy(buf, bd->data, buf_size);
    bd->data += buf_size;
    bd->size -= buf_size;

    return buf_size;
}

bool VideoPlayer::getHasVideo() const
{
    return hasVideo;
}

void VideoPlayer::setHasVideo(bool value)
{
    hasVideo = value;
}

bool VideoPlayer::getHasAudio() const
{
    return hasAudio;
}

void VideoPlayer::setHasAudio(bool value)
{
    hasAudio = value;
}

void VideoPlayer::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    int width = this->width();
    int height = this->height();

    painter->setBrush(Qt::black);
    painter->drawRect(0, 0, width, height);

    if (!image_.isNull()) {
        QImage img = image_.scaled(QSize(width, height), Qt::KeepAspectRatio);
        /* calculate display position */
        int x = (this->width() - img.width()) / 2;
        int y = (this->height() - img.height()) / 2;
        painter->drawImage(QPoint(x, y), img);
    }
}

int VideoPlayer::tsDecode(void *arg)
{
    VideoPlayer* player = (VideoPlayer*) arg;
    HlsIndex* index = player->getHlsIndex();
    QList<TsFile*> files = index->getFiles();
    player->setDecodeIndex(0);
    while (player->getState() != STOP) {
        int decodeIndex = player->getDecodeIndex();
        TsFile* file = files[decodeIndex];
        AVFormatContext *fmt_ctx = nullptr;
        AVIOContext *avio_ctx = nullptr;
        AVCodec* video_codec_  = nullptr;
        AVCodecContext* video_codec_context_  = nullptr;
        AVStream* video_stream_  = nullptr;
        AVCodec* audio_codec_  = nullptr;
        AVCodecContext* audio_codec_context_  = nullptr;
        AVStream* audio_stream_  = nullptr;
        // io缓冲初始化
        uint8_t *avio_ctx_buffer = nullptr;
        size_t avio_ctx_buffer_size = 4096;

        int video_index_ = -1;
        int audio_index_ = -1;
        // ts文件参数初始化
        buffer_data bd = {(uint8_t*)file->data(), (size_t)file->dataLength()};

        fmt_ctx = avformat_alloc_context();
        avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
        avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                      0, &bd, read_packet, NULL, NULL);
        fmt_ctx->pb = avio_ctx;
        //fmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
        if (avformat_open_input(&fmt_ctx, "", NULL, NULL) < 0)
        {
            fprintf(stderr, "Could not open input\n");
            return 0 ;
        }

        for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_index_ = i;
                qDebug() << "Find video stream.videoIndex: " << video_index_;
                continue;
            }
        }
        if (video_index_ >= 0) {
            player->setHasVideo(true);
            video_stream_ = fmt_ctx->streams[video_index_];
            // 配置packet舍弃策略
            fmt_ctx->streams[video_index_]->discard = AVDISCARD_DEFAULT;
            // 根据视频流创建视频解码器上下文
            video_codec_context_ = avcodec_alloc_context3(NULL);
            if (avcodec_parameters_to_context(
                        video_codec_context_,
                        fmt_ctx->streams[video_index_]->codecpar) < 0) {
                qDebug() << "Fill video decoder context failed.";
                return -1;
            }
            // 寻找视频解码器
            video_codec_ = avcodec_find_decoder(video_codec_context_->codec_id);
            if (video_codec_ == NULL) {
                qDebug() << "Video decoder not found.";
                return -1;
            }
            // 打开视频解码器待用
            if (avcodec_open2(video_codec_context_, video_codec_, NULL) < 0) {
                qDebug() << "Open video decoder failed.";
                return -1;
            }
        }
        if (audio_index_ >= 0) {
            player->setHasAudio(true);
            audio_stream_ = fmt_ctx->streams[audio_index_];
            fmt_ctx->streams[audio_index_]->discard = AVDISCARD_DEFAULT;
            audio_codec_context_ = avcodec_alloc_context3(NULL);
            if (avcodec_parameters_to_context(
                        audio_codec_context_,
                        fmt_ctx->streams[audio_index_]->codecpar) < 0) {
                qDebug() << "Fill audio decoder failed.";
                return -1;
            }
            audio_codec_ = avcodec_find_decoder(audio_codec_context_->codec_id);
            if (audio_codec_ == NULL) {
                qDebug() << "Audio Decoder not found.";
                return -1;
            }
            if (avcodec_open2(audio_codec_context_, audio_codec_, NULL) < 0) {
                qDebug() << "Open audio decoder failed.";
                return -1;
            }
        }
        AVPacket pkt;
        AVFrame* frame = av_frame_alloc();
        while (1) {
            int ret = 0;
            ret = av_read_frame(fmt_ctx, &pkt);
            if (pkt.stream_index != video_index_ &&
                    pkt.stream_index != audio_index_) {
                av_packet_unref(&pkt);
                continue;
            }
            if (ret < 0) {
                //qDebug() << av_err2str(ret);
                if (ret == -541478725) {
                    break;
                }
                else {
                    continue;
                }
            }
            avcodec_send_packet(video_codec_context_, &pkt);
            ret = avcodec_receive_frame(video_codec_context_, frame);
            if (pkt.stream_index == video_index_) {
                player->videoQueue_->enqueue(frame);
            }
            else {
                player->audioQueue_->enqueue(frame);
            }
            av_packet_unref(&pkt);
        }
        avformat_free_context(fmt_ctx);
        av_frame_free(&frame);
        decodeIndex += 1;
        player->setDecodeIndex(decodeIndex);
    }
    return 0;
}

int VideoPlayer::videoPlay(void *arg)
{
    VideoPlayer* player = (VideoPlayer*) arg;
    AVFrame* frame;
    AVFrame* frame_rgb = av_frame_alloc();
    while (player->state_ != STOP) {
        while (player->state_ == PAUSE) {
            SDL_Delay(5);
            continue;
        }
        if (player->videoQueue_->queueSize() <= 0) {
            continue;
        }
        frame = player->videoQueue_->dequeue();
        if (player->swsContext_ == nullptr) {
            player->swsContext_ = sws_alloc_context();
            player->swsContext_ = sws_getContext(frame->width,
                                                 frame->height,
                                                 AVPixelFormat::AV_PIX_FMT_YUV420P,
                                                 frame->width,
                                                 frame->height,
                                                 AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr,
                                                 nullptr, nullptr);
            player->swsOutBuffer_ = (unsigned char*)av_malloc(
                        av_image_get_buffer_size(AV_PIX_FMT_RGBA,
                                                 frame->width,
                                                 frame->height,
                                                 1));
        }
        if (player->swsContext_ != nullptr) {
            av_image_fill_arrays(frame_rgb->data,
                                 frame_rgb->linesize,
                                 player->swsOutBuffer_, AV_PIX_FMT_RGBA,
                                 frame->width,
                                 frame->height,
                                 1);
            sws_scale(player->swsContext_, (const unsigned char* const*)frame->data,
                      frame->linesize, 0, frame->height,
                      frame_rgb->data, frame_rgb->linesize);
        }
        QImage img = (QImage(frame_rgb->data[0],
                      frame->width,
                      frame->height,
                      QImage::Format_RGBA8888//QImage::Format_RGBX64
                      ).copy());
    }
}

void VideoPlayer::onM3u8ReplyDone(bool success, HlsIndex *index)
{
    if (success) {
        index_ = index;
    }
    else {
        index_ = nullptr;
    }
    tsFetcher_ = SDL_CreateThread(tsFetch, "tsFetch", this);
    tsDecoder_ = SDL_CreateThread(tsDecode, "tsDecode", this);
    if (hasVideo) {
        videoThread_ = SDL_CreateThread(videoPlay, "videoPlay", this);
    }
}

void VideoPlayer::onMediaInfoReplyDone(bool success, int nbFrames, int frameRate)
{
    if (success && index_ != nullptr) {
        index_->setNbFrames(nbFrames);
        index_->setFrameRate(frameRate);
    }
}

void VideoPlayer::onUpdateFrame(QImage image)
{
    image_ = image;
    update();
}
