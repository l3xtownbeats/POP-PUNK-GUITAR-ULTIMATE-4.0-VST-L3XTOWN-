#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ppgu::performance
{
    // Song sections are a distinct musical axis; they are never pattern slots.
    enum class SongSection : std::uint8_t
    {
        Intro = 0,
        Verse,
        Chorus,
        Bridge,
        Outro,
        Count
    };

    class PatternSectionModel final
    {
    public:
        static constexpr std::size_t sectionCount = static_cast<std::size_t>(SongSection::Count);
        static constexpr int minimumPatternSlot = 0;
        static constexpr int maximumPatternSlot = 7;
        static constexpr std::int8_t inheritLegacySlot = -1;
        static constexpr std::uint32_t currentSchemaVersion = 1;

        struct Snapshot
        {
            std::uint32_t schemaVersion = currentSchemaVersion;
            SongSection selectedSection = SongSection::Intro;
            std::array<std::int8_t, sectionCount> patternSlots {{
                inheritLegacySlot, inheritLegacySlot, inheritLegacySlot,
                inheritLegacySlot, inheritLegacySlot
            }};
        };

        constexpr PatternSectionModel() noexcept = default;

        static constexpr bool isValidSection(SongSection section) noexcept
        {
            return static_cast<std::size_t>(section) < sectionCount;
        }

        static constexpr bool isValidPatternSlot(int slot) noexcept
        {
            return slot >= minimumPatternSlot && slot <= maximumPatternSlot;
        }

        bool selectSection(SongSection section) noexcept
        {
            if (!isValidSection(section))
                return false;
            selectedSection_ = section;
            clearPreview();
            return true;
        }

        SongSection selectedSection() const noexcept { return selectedSection_; }

        // Preview is an audition-only draft. It does not modify the saved
        // section assignment and never changes the legacy APVTS parameter.
        bool beginPreview(int patternSlot) noexcept
        {
            if (!isValidPatternSlot(patternSlot))
                return false;
            previewSection_ = selectedSection_;
            previewPatternSlot_ = static_cast<std::int8_t>(patternSlot);
            previewActive_ = true;
            return true;
        }

        bool hasPreview() const noexcept { return previewActive_; }

        int previewPatternSlot() const noexcept
        {
            return previewActive_ ? static_cast<int>(previewPatternSlot_) : -1;
        }

        bool applyPreview() noexcept
        {
            if (!previewActive_ || !isValidSection(previewSection_)
                || !isValidPatternSlot(previewPatternSlot_))
                return false;
            assignments_[static_cast<std::size_t>(previewSection_)] = previewPatternSlot_;
            clearPreview();
            return true;
        }

        void cancelPreview() noexcept { clearPreview(); }

        bool assignPattern(SongSection section, int patternSlot) noexcept
        {
            if (!isValidSection(section) || !isValidPatternSlot(patternSlot))
                return false;
            assignments_[static_cast<std::size_t>(section)] = static_cast<std::int8_t>(patternSlot);
            clearPreview();
            return true;
        }

        bool clearAssignment(SongSection section) noexcept
        {
            if (!isValidSection(section))
                return false;
            assignments_[static_cast<std::size_t>(section)] = inheritLegacySlot;
            clearPreview();
            return true;
        }

        int assignedPatternSlot(SongSection section) const noexcept
        {
            if (!isValidSection(section))
                return -1;
            return static_cast<int>(assignments_[static_cast<std::size_t>(section)]);
        }

        // Old projects have only the global patternSlot. Unassigned sections
        // inherit that value, so loading an old session preserves its sound.
        int effectivePatternSlot(SongSection section, int legacyPatternSlot) const noexcept
        {
            if (!isValidSection(section))
                return isValidPatternSlot(legacyPatternSlot) ? legacyPatternSlot : 0;
            const int assigned = assignedPatternSlot(section);
            if (isValidPatternSlot(assigned))
                return assigned;
            return isValidPatternSlot(legacyPatternSlot) ? legacyPatternSlot : 0;
        }

        Snapshot save() const noexcept
        {
            Snapshot snapshot;
            snapshot.selectedSection = selectedSection_;
            snapshot.patternSlots = assignments_;
            return snapshot;
        }

        // Schema 0 / absent extension means a legacy project: retain all
        // sections as "inherit legacy patternSlot". Invalid fields fail closed.
        void restore(const Snapshot& snapshot) noexcept
        {
            selectedSection_ = isValidSection(snapshot.selectedSection)
                ? snapshot.selectedSection : SongSection::Intro;
            assignments_.fill(inheritLegacySlot);
            if (snapshot.schemaVersion >= currentSchemaVersion)
            {
                for (std::size_t i = 0; i < sectionCount; ++i)
                    if (isValidPatternSlot(snapshot.patternSlots[i]))
                        assignments_[i] = snapshot.patternSlots[i];
            }
            clearPreview();
        }

    private:
        void clearPreview() noexcept
        {
            previewActive_ = false;
            previewSection_ = SongSection::Intro;
            previewPatternSlot_ = inheritLegacySlot;
        }

        SongSection selectedSection_ = SongSection::Intro;
        std::array<std::int8_t, sectionCount> assignments_ {{
            inheritLegacySlot, inheritLegacySlot, inheritLegacySlot,
            inheritLegacySlot, inheritLegacySlot
        }};
        SongSection previewSection_ = SongSection::Intro;
        std::int8_t previewPatternSlot_ = inheritLegacySlot;
        bool previewActive_ = false;
    };
}
