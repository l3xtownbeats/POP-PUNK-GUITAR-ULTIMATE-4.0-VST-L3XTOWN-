#pragma once
namespace ppgu::ui
{
class SkinGallerySelection final
{
public:
    explicit SkinGallerySelection(int originalIndex) noexcept
        : originalIndex_(originalIndex), draftIndex_(originalIndex) {}
    bool select(int index) noexcept
    {
        if (finalized_ || index < 1) return false;
        draftIndex_ = index; return true;
    }
    bool markPreviewed() noexcept
    {
        if (finalized_ || draftIndex_ < 1) return false;
        previewed_ = true; return true;
    }
    bool commit() noexcept
    {
        if (finalized_ || draftIndex_ < 1) return false;
        finalized_ = true; return true;
    }
    int cancel() noexcept
    {
        if (finalized_) return originalIndex_;
        finalized_ = true; return originalIndex_;
    }
    bool shouldRestoreOnClose() const noexcept { return previewed_ && !finalized_; }
    int originalIndex() const noexcept { return originalIndex_; }
    int draftIndex() const noexcept { return draftIndex_; }
    bool isFinalized() const noexcept { return finalized_; }
private:
    int originalIndex_ = 0, draftIndex_ = 0;
    bool previewed_ = false, finalized_ = false;
};
}