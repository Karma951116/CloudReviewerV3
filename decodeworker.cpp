#include "decodeworker.h"
#include <QDebug>

DecodeWorker::DecodeWorker(HlsIndex *hlsIndex, int blockIndex)
{
    hlsIndex_ = hlsIndex;
    blockIndex_ = blockIndex;
}

void DecodeWorker::run()
{
    TsFile* ts = hlsIndex_->getFiles().at(blockIndex_);

    if (ts->fetchState() != TsFile::FETCHED) {
        return;
    }
    ts->setDecodeState(TsFile::DECODING);
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
    size_t avio_ctx_buffer_size = 65536;

    int video_index_ = -1;
    int audio_index_ = -1;
    // ts文件参数初始化
    buffer_data bd = {(uint8_t*)ts->data(), ts->dataLength(), 0};

    fmt_ctx = avformat_alloc_context();
    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
    avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                  0, &bd, read_packet, NULL, NULL);
    fmt_ctx->pb = avio_ctx;
    fmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
    if (avformat_open_input(&fmt_ctx, "", NULL, NULL) < 0)
    {
        fprintf(stderr, "Could not open input\n");
        return;
    }

    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index_ = i;
            //qDebug() << "Find video stream.videoIndex: " << video_index_;
            continue;
        }
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index_ = i;
            //qDebug() << "Find video stream.audio_index_: " << audio_index_;
            continue;
        }
    }
    if (video_index_ >= 0) {
        video_stream_ = fmt_ctx->streams[video_index_];
        // 配置packet舍弃策略
        fmt_ctx->streams[video_index_]->discard = AVDISCARD_DEFAULT;
        // 根据视频流创建视频解码器上下文
        video_codec_context_ = avcodec_alloc_context3(NULL);
        if (avcodec_parameters_to_context(
                    video_codec_context_,
                    fmt_ctx->streams[video_index_]->codecpar) < 0) {
            qDebug() << "Fill video decoder context failed.";
            return;
        }
        // 寻找视频解码器
        video_codec_ = avcodec_find_decoder(video_codec_context_->codec_id);
        if (video_codec_ == NULL) {
            qDebug() << "Video decoder not found.";
            return;
        }
        // 打开视频解码器待用
        if (avcodec_open2(video_codec_context_, video_codec_, NULL) < 0) {
            qDebug() << "Open video decoder failed.";
            return;
        }
    }
    if (audio_index_ >= 0) {
        audio_stream_ = fmt_ctx->streams[audio_index_];
        fmt_ctx->streams[audio_index_]->discard = AVDISCARD_DEFAULT;
        audio_codec_context_ = avcodec_alloc_context3(NULL);
        if (avcodec_parameters_to_context(
                    audio_codec_context_,
                    fmt_ctx->streams[audio_index_]->codecpar) < 0) {
            qDebug() << "Fill audio decoder failed.";
            return;
        }
        audio_codec_ = avcodec_find_decoder(audio_codec_context_->codec_id);
        if (audio_codec_ == NULL) {
            qDebug() << "Audio Decoder not found.";
            return;
        }
        if (avcodec_open2(audio_codec_context_, audio_codec_, NULL) < 0) {
            qDebug() << "Open audio decoder failed.";
            return;
        }
    }

    AVPacket pkt;
    QVector<AVFrame*>* vFrameVector = new QVector<AVFrame*>();
    QVector<AVFrame*>* aFrameVector = new QVector<AVFrame*>();
    int ret = 0;
    QVariant vMeta;
    QVariant aMeta;
    while (1) {
        ret = 0;
        ret = av_read_frame(fmt_ctx, &pkt);
        if (pkt.stream_index != video_index_ &&
                pkt.stream_index != audio_index_) {
            av_packet_unref(&pkt);
            continue;
        }
        AVFrame* frame = av_frame_alloc();
        ret = avcodec_send_packet(
                    pkt.stream_index == video_index_ ? video_codec_context_ : audio_codec_context_,
                    &pkt);
        if (ret == AVERROR_EOF) {
            av_packet_unref(&pkt);
            av_frame_free(&frame);
            break;
        }
        else if (ret == AVERROR(EAGAIN)) {
            av_packet_unref(&pkt);
            av_frame_free(&frame);
            continue;
        }
        ret = avcodec_receive_frame(
                    pkt.stream_index == video_index_ ? video_codec_context_ : audio_codec_context_,
                    frame);
        if (ret == AVERROR_EOF) {
            av_packet_unref(&pkt);
            av_frame_free(&frame);
            break;
        }
        else if (ret == AVERROR(EAGAIN)) {
            av_packet_unref(&pkt);
            av_frame_free(&frame);
            continue;
        }
        if (pkt.stream_index == video_index_) {
            if (blockIndex_ == 0 && vMeta.isNull()) {
                VideoMeta meta_;
                meta_.width = video_codec_context_->width == 0 ?
                            frame->width : video_codec_context_->width;
                meta_.height = video_codec_context_->height == 0 ?
                            frame->height : video_codec_context_->height;
                meta_.pixelFormat = video_codec_context_->pix_fmt;
                meta_.vTimeBase = av_q2d(video_stream_->time_base);
                meta_.vTimeBaseRational = video_stream_->time_base;
                // 应从接口取得，仅为测试设置
                meta_.frameRate = 24;
                vMeta.setValue(meta_);
            }
            vFrameVector->append(frame);
        }
        else if (pkt.stream_index == audio_index_){
            if (blockIndex_ == 0 && aMeta.isNull()) {
                AudioMeta meta_;
                meta_.channelLayout = audio_codec_context_->channel_layout == 0 ?
                            frame->channel_layout : audio_codec_context_->channel_layout;
                meta_.sampleRate = audio_codec_context_->sample_rate == 0 ?
                            frame->sample_rate : audio_codec_context_->sample_rate;
                meta_.sampleFormat = audio_codec_context_->sample_fmt;
                meta_.channels = audio_codec_context_->channels == 0 ?
                            frame->channels : audio_codec_context_->channels;
                meta_.aTimeBase = av_q2d(audio_stream_->time_base);
                meta_.aTimeBaseRational = audio_stream_->time_base;
                meta_.samplePerFrame = frame->nb_samples;
                aMeta.setValue(meta_);
            }
            aFrameVector->append(frame);
        }
        av_packet_unref(&pkt);
    }
    avio_context_free(&avio_ctx);
    avcodec_free_context(&video_codec_context_);
    avcodec_free_context(&audio_codec_context_);
    avformat_free_context(fmt_ctx);
    qDebug() << "Decode Ts: " << ts->fileName();
    emit decodeFinished(vFrameVector, aFrameVector, blockIndex_, vMeta, aMeta);
    ts->setDecodeState(TsFile::DECODED);
    this->deleteLater();
}
int DecodeWorker::read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    buffer_data *bd = (buffer_data *) opaque;
    int copySize = bd->offset + buf_size < bd->size ? buf_size :
                                                      (bd->size - bd->offset);
    buf_size = FFMIN(buf_size, (int)bd->size);
    if (bd->offset < bd->size) {
        memcpy(buf, bd->data + bd->offset, copySize);
        bd->offset += copySize;
        return copySize;
    }
    else {
        return -1;
    }
}
