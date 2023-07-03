#include "tsroll.h"

TsRoll::TsRoll(int blockCount)
    : tsBlockIndex_(0),
      tsFrameIndex_(0)
{
    roll = QVector<QVector<AVFrame*>*>(blockCount);
    for (int i = 0; i < roll.size(); i++) {
        roll[i] = nullptr;
    }
    connect(this, SIGNAL(clearBuffer()),
            this, SLOT(onClearBuffer()));
}

void TsRoll::setTsBlock(int index, QVector<AVFrame*>* block)
{
    QWriteLocker locker(&lock);
    roll[index] = block;
}

bool TsRoll::isBlockEmpty(int blockIdx)
{
    if (!hasBlock(blockIdx)) {
        return true;
    }
    else if (roll[blockIdx]->size() == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool TsRoll::isEmpty()
{
    return roll.size() == 0;
}

int TsRoll::blockSize(int blockIdx)
{
    return hasBlock(blockIdx) ? roll[blockIdx]->size() : 0;
}

int TsRoll::size()
{
    return roll.size();
}

void TsRoll::emptyBlock(int blockIdx)
{
    QWriteLocker locker(&lock);
    QVector<AVFrame*>* block = roll.at(blockIdx);
    if (block != nullptr) {
        for(int i = 0; i < block->size(); i++) {
            AVFrame* frame = block->at(i);
            if (frame != nullptr) {
               av_frame_free(&frame);
            }
        }
    }
    roll[blockIdx] = nullptr;
    delete  block;
    blockCleared(blockIdx);
    return;
}

void TsRoll::empty()
{
    QWriteLocker locker(&lock);
    for(int i = 0; i < roll.size(); i++) {
        QVector<AVFrame*>* block = roll.at(i);
        if (block != nullptr) {
            for(int j = 0; j < block->size(); j++) {
                AVFrame* frame = block->at(j);
                if (frame != nullptr) {
                   av_frame_free(&frame);
                }
            }
            block->clear();
        }
        roll[i] = nullptr;
        delete block;
        blockCleared(i);
    }
}

AVFrame *TsRoll::read(int blockIdx, int frameIdx)
{
    if (blockIdx < 0 || blockIdx >= roll.size()) {
        return nullptr;
    }
    if (!hasBlock(blockIdx)) {
        return nullptr;
    }
    if (frameIdx < 0 ||frameIdx >= roll[blockIdx]->size()) {
        return nullptr;
    }
    //QReadLocker locker(&lock);
    AVFrame* frame = roll.at(blockIdx)->at(frameIdx);
    return frame;
}

AVFrame *TsRoll::read()
{
    //QReadLocker locker(&lock);
    AVFrame* frame = roll.at(tsBlockIndex_)->at(tsFrameIndex_);
    return frame;
}

void TsRoll::nextFrame()
{
    if (roll.at(tsBlockIndex_) == nullptr) {
        return;
    }
    if (roll.at(tsBlockIndex_)->size() - 1 < tsFrameIndex_) {
        if (!isLastBlock()) {
            nextBlock();
            tsFrameIndex_ = 0;
        }
        return;
    }
    if (tsFrameIndex_ + 1 < roll.at(tsBlockIndex_)->size()) {
        tsFrameIndex_ += 1;
    }
    else {
        if (!isLastBlock()) {
            nextBlock();
            tsFrameIndex_ = 0;
        }
    }
}

void TsRoll::previousFrame()
{
    if (tsFrameIndex_ - 1 >= 0) {
        tsFrameIndex_ -= 1;
    }
    else {
        if (!isFirstBlock()) {
            previousBlock();
            tsFrameIndex_ = roll.at(tsBlockIndex_)->size() - 1;
        }
    }
}

void TsRoll::nextBlock()
{
    if (tsBlockIndex_ + 1 < roll.size()){
        tsBlockIndex_ += 1;
    }
    clearBuffer();
}

void TsRoll::previousBlock()
{
    if (tsBlockIndex_ - 1 >= 0) {
        tsBlockIndex_ -= 1;
    }
}

bool TsRoll::isFirstFrame()
{
    return tsBlockIndex_ == 0 && tsFrameIndex_ == 0;
}

bool TsRoll::isLastFrame()
{
    return tsBlockIndex_ == (roll.size() - 1) &&
            tsFrameIndex_ == (roll.at(tsBlockIndex_)->size() - 1);
}

bool TsRoll::isFirstBlock()
{
    return tsBlockIndex_ == 0;
}

bool TsRoll::isLastBlock()
{
    return tsBlockIndex_ == (roll.size() - 1);
}

bool TsRoll::seek(int blockIdx, int frameIdx)
{
//    if (!canRead(blockIdx, frameIdx)) {
//        return false;
//    }
//    else {

//    }
    tsBlockIndex_ = blockIdx;
    tsFrameIndex_ = frameIdx;
    return true;
}

bool TsRoll::hasBlock(int blockIdx)
{
    if (blockIdx > roll.length() - 1) {
        return false;
    }
    if (blockIdx < 0) {
        return false;
    }
    if (roll.length() > 0) {
        return roll.at(blockIdx) != nullptr;
    }
    else {
        return false;
    }
}

bool TsRoll::canRead(int blockIdx, int frameIdx)
{
    if (blockIdx < 0 || blockIdx >= roll.size()) {
        return false;
    }
    if (!hasBlock(blockIdx)) {
        return false;
    }
    if (frameIdx < 0 ||frameIdx >= roll[blockIdx]->size()) {
        return false;
    }
    return roll.at(blockIdx)->at(frameIdx) != nullptr;

}

bool TsRoll::canRead()
{
    if (roll.isEmpty()) {
        return false;
    }
    else if (roll.at(tsBlockIndex_) == nullptr) {
        return false;
    }
    else {
        if (roll.at(tsBlockIndex_)->isEmpty()) {
            return false;
        }
        else {
            if (roll.at(tsBlockIndex_)->size() - 1 < tsFrameIndex_) {
                return false;
            }
            else {
                return roll.at(tsBlockIndex_)->at(tsFrameIndex_) != nullptr;
            }
        }
    }
}

int TsRoll::tsBlockIndex() const
{
    return tsBlockIndex_;
}

int TsRoll::tsFrameIndex() const
{
    return tsFrameIndex_;
}

void TsRoll::onClearBuffer()
{
    int target = tsBlockIndex_ - 2;
    if (target < 0) {
        return;
    }
    if (isBlockEmpty(target)) {
        return;
    }
    emptyBlock(target);
    blockCleared(target);
}

