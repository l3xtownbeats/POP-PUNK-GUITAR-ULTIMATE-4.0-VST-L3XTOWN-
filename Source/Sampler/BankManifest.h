#pragma once
#include <juce_core/juce_core.h>
#include <cstdint>
#include <string>
#include <vector>
namespace ppgu::sampler {
// Conflict-check REV10: immutable control-plane metadata; never used by processBlock().
struct BankLicenseEvidence {
 std::string sourceUrl, author, licenseId="UNKNOWN", licenseUrl, evidencePath, verifiedAtUtc, evidenceNote;
 bool redistributionAllowed=false;
};
struct BankSampleRecord {
 std::string sampleId, relativePath, sha256, status="OK";
 std::uint64_t fileBytes=0, frames=0;
 std::int64_t modificationTimeMs=0;
 double sampleRate=0.0;
 unsigned int channels=0, bitsPerSample=0;
 bool floatingPoint=false;
};
struct BankManifest {
 static constexpr int currentSchemaVersion=1;
 int schemaVersion=currentSchemaVersion;
 std::string bankId, displayName, canonicalRoot, scannedAtUtc, scannerVersion="PPGU-SB1";
 BankLicenseEvidence license;
 std::vector<BankSampleRecord> samples;
 [[nodiscard]] juce::var toJson() const;
 [[nodiscard]] static bool fromJson(const juce::var&, BankManifest&, std::string&);
 [[nodiscard]] bool validate(std::string&) const;
};
}