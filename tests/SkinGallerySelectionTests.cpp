#include "UI/SkinGallerySelection.h"
#include <cassert>
int main()
{
    using ppgu::ui::SkinGallerySelection;
    {
        SkinGallerySelection state(53);
        assert(state.originalIndex() == 53 && state.draftIndex() == 53);
        assert(state.select(54) && state.draftIndex() == 54);
        assert(!state.shouldRestoreOnClose());
        assert(state.markPreviewed() && state.shouldRestoreOnClose());
        assert(state.cancel() == 53 && state.isFinalized());
        assert(!state.shouldRestoreOnClose() && !state.select(55));
    }
    {
        SkinGallerySelection state(53);
        assert(state.select(54) && state.markPreviewed());
        assert(state.commit() && state.isFinalized());
        assert(!state.shouldRestoreOnClose() && !state.commit());
    }
    {
        SkinGallerySelection state(53);
        assert(!state.select(0) && !state.markPreviewed() && !state.commit());
        assert(state.cancel() == 53);
    }
    return 0;
}