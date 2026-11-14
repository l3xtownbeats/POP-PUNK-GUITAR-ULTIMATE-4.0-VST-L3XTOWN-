#include "PatternSectionModel.h"

#include <iostream>

using ppgu::performance::PatternSectionModel;
using ppgu::performance::SongSection;

int main()
{
    PatternSectionModel model;

    // Section selection is independent from the eight legacy rhythm slots.
    if (PatternSectionModel::sectionCount != 5
        || model.selectedSection() != SongSection::Intro)
        return 1;
    if (!model.selectSection(SongSection::Verse)
        || model.selectedSection() != SongSection::Verse)
        return 2;
    if (model.selectSection(static_cast<SongSection>(255)))
        return 3;

    // Preview is non-committing; Apply creates a section-specific association.
    if (!model.beginPreview(3) || !model.hasPreview()
        || model.previewPatternSlot() != 3
        || model.assignedPatternSlot(SongSection::Verse) != PatternSectionModel::inheritLegacySlot)
        return 4;
    if (model.effectivePatternSlot(SongSection::Verse, 5) != 5)
        return 5;
    if (!model.applyPreview() || model.hasPreview()
        || model.assignedPatternSlot(SongSection::Verse) != 3
        || model.effectivePatternSlot(SongSection::Verse, 5) != 3)
        return 6;

    // Other sections remain independent and continue inheriting legacy state.
    if (model.effectivePatternSlot(SongSection::Chorus, 6) != 6)
        return 7;
    if (!model.assignPattern(SongSection::Chorus, 7)
        || model.effectivePatternSlot(SongSection::Chorus, 1) != 7)
        return 8;

    // Back/cancel discards only the draft.
    if (!model.beginPreview(2) || !model.selectSection(SongSection::Bridge)
        || model.hasPreview()
        || model.assignedPatternSlot(SongSection::Verse) != 3)
        return 9;
    if (!model.beginPreview(4))
        return 10;
    model.cancelPreview();
    if (model.hasPreview()
        || model.assignedPatternSlot(SongSection::Bridge) != PatternSectionModel::inheritLegacySlot)
        return 11;

    // Invalid slots and invalid section writes are rejected without mutation.
    if (model.beginPreview(-1) || model.beginPreview(8)
        || model.assignPattern(SongSection::Outro, 8)
        || model.assignPattern(static_cast<SongSection>(255), 1))
        return 12;

    // Save/restore round-trip preserves selected section and all associations.
    const auto saved = model.save();
    PatternSectionModel restored;
    restored.restore(saved);
    if (restored.selectedSection() != SongSection::Bridge
        || restored.assignedPatternSlot(SongSection::Verse) != 3
        || restored.assignedPatternSlot(SongSection::Chorus) != 7
        || restored.effectivePatternSlot(SongSection::Outro, 2) != 2)
        return 13;

    // Legacy projects (schema 0) inherit their original global patternSlot.
    PatternSectionModel::Snapshot legacy;
    legacy.schemaVersion = 0;
    legacy.selectedSection = SongSection::Chorus;
    legacy.patternSlots.fill(6);
    restored.restore(legacy);
    for (std::size_t i = 0; i < PatternSectionModel::sectionCount; ++i)
    {
        const auto section = static_cast<SongSection>(i);
        if (restored.assignedPatternSlot(section) != PatternSectionModel::inheritLegacySlot
            || restored.effectivePatternSlot(section, 6) != 6)
            return 14;
    }
    if (restored.selectedSection() != SongSection::Chorus)
        return 15;

    // Corrupt current-schema entries are sanitized; valid entries survive.
    auto corrupt = saved;
    corrupt.patternSlots[0] = 99;
    corrupt.patternSlots[1] = -2;
    restored.restore(corrupt);
    if (restored.assignedPatternSlot(SongSection::Intro) != PatternSectionModel::inheritLegacySlot
        || restored.assignedPatternSlot(SongSection::Verse) != PatternSectionModel::inheritLegacySlot
        || restored.assignedPatternSlot(SongSection::Chorus) != 7)
        return 16;

    std::cout << "PatternSectionModel tests passed: section isolation, preview/apply/cancel, persistence, legacy fallback.\n";
    return 0;
}
