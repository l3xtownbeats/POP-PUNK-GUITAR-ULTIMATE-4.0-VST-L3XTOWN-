#include "BankManifest.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <memory>
#include <filesystem>
namespace ppgu::sampler {
namespace {
juce::var sv(const std::string& s){ return juce::String(s); }
std::string get(const juce::DynamicObject& o,const char* k){return o.getProperty(k).toString().toStdString();}
bool hashOk(const std::string& s){return s.size()==64 && std::all_of(s.begin(),s.end(),[](unsigned char c){return std::isxdigit(c)!=0;});}
bool safePath(const std::string& s){
 if(s.empty()||s.find('\\')!=std::string::npos||s.find('\0')!=std::string::npos)return false;
 std::filesystem::path p(s); if(p.is_absolute()||p.has_root_name()||p.has_root_directory())return false;
 for(const auto& part:p) if(part==".."||part=="."||part.empty())return false;
 return true;
}
}
juce::var BankManifest::toJson() const {
 auto root=std::make_unique<juce::DynamicObject>();
 root->setProperty("schemaVersion",schemaVersion); root->setProperty("bankId",sv(bankId));
 root->setProperty("displayName",sv(displayName)); root->setProperty("canonicalRoot",sv(canonicalRoot));
 root->setProperty("scannedAtUtc",sv(scannedAtUtc)); root->setProperty("scannerVersion",sv(scannerVersion));
 auto lic=std::make_unique<juce::DynamicObject>();
 lic->setProperty("sourceUrl",sv(license.sourceUrl)); lic->setProperty("author",sv(license.author));
 lic->setProperty("licenseId",sv(license.licenseId)); lic->setProperty("licenseUrl",sv(license.licenseUrl));
 lic->setProperty("evidencePath",sv(license.evidencePath)); lic->setProperty("verifiedAtUtc",sv(license.verifiedAtUtc));
 lic->setProperty("evidenceNote",sv(license.evidenceNote)); lic->setProperty("redistributionAllowed",license.redistributionAllowed);
 root->setProperty("license",juce::var(lic.release()));
 juce::Array<juce::var> entries;
 for(const auto& s:samples){auto o=std::make_unique<juce::DynamicObject>();
  o->setProperty("sampleId",sv(s.sampleId)); o->setProperty("relativePath",sv(s.relativePath)); o->setProperty("sha256",sv(s.sha256));
  o->setProperty("status",sv(s.status)); o->setProperty("fileBytes",juce::var(static_cast<juce::int64>(s.fileBytes)));
  o->setProperty("frames",juce::var(static_cast<juce::int64>(s.frames))); o->setProperty("modificationTimeMs",juce::var(static_cast<juce::int64>(s.modificationTimeMs)));
  o->setProperty("sampleRate",s.sampleRate); o->setProperty("channels",static_cast<int>(s.channels));
  o->setProperty("bitsPerSample",static_cast<int>(s.bitsPerSample)); o->setProperty("floatingPoint",s.floatingPoint);
  entries.add(juce::var(o.release()));}
 root->setProperty("samples",entries); return juce::var(root.release());
}
bool BankManifest::fromJson(const juce::var& v,BankManifest& dst,std::string& err){
 // Conflict-check: parse into a candidate; failure never mutates the published destination.
 if(!v.isObject()){err="Manifest root must be an object.";return false;}
 const auto* o=v.getDynamicObject(); BankManifest c;
 c.schemaVersion=static_cast<int>(o->getProperty("schemaVersion")); c.bankId=get(*o,"bankId");
 c.displayName=get(*o,"displayName"); c.canonicalRoot=get(*o,"canonicalRoot");
 c.scannedAtUtc=get(*o,"scannedAtUtc"); c.scannerVersion=get(*o,"scannerVersion");
 const auto lv=o->getProperty("license"); if(!lv.isObject()){err="License evidence is missing.";return false;}
 const auto* l=lv.getDynamicObject(); c.license.sourceUrl=get(*l,"sourceUrl"); c.license.author=get(*l,"author");
 c.license.licenseId=get(*l,"licenseId"); c.license.licenseUrl=get(*l,"licenseUrl");
 c.license.evidencePath=get(*l,"evidencePath"); c.license.verifiedAtUtc=get(*l,"verifiedAtUtc");
 c.license.evidenceNote=get(*l,"evidenceNote"); c.license.redistributionAllowed=static_cast<bool>(l->getProperty("redistributionAllowed"));
 const auto a=o->getProperty("samples"); if(!a.isArray()){err="Samples must be an array.";return false;}
 for(const auto& vEntry:*a.getArray()){if(!vEntry.isObject()){err="Sample entry must be an object.";return false;}
  const auto* x=vEntry.getDynamicObject(); BankSampleRecord s; s.sampleId=get(*x,"sampleId"); s.relativePath=get(*x,"relativePath");
  s.sha256=get(*x,"sha256"); s.status=get(*x,"status"); s.fileBytes=static_cast<std::uint64_t>(static_cast<juce::int64>(x->getProperty("fileBytes")));
  s.frames=static_cast<std::uint64_t>(static_cast<juce::int64>(x->getProperty("frames")));
  s.modificationTimeMs=static_cast<std::int64_t>(static_cast<juce::int64>(x->getProperty("modificationTimeMs")));
  s.sampleRate=static_cast<double>(x->getProperty("sampleRate")); s.channels=static_cast<unsigned int>(static_cast<int>(x->getProperty("channels")));
  s.bitsPerSample=static_cast<unsigned int>(static_cast<int>(x->getProperty("bitsPerSample"))); s.floatingPoint=static_cast<bool>(x->getProperty("floatingPoint"));
  c.samples.push_back(std::move(s));}
 if(!c.validate(err))return false; dst=std::move(c); return true;
}
bool BankManifest::validate(std::string& err) const {
 if(schemaVersion!=currentSchemaVersion){err="Unsupported manifest schema version.";return false;}
 if(bankId.empty()||displayName.empty()||canonicalRoot.empty()){err="Bank identity, display name and root are required.";return false;}
 if(license.licenseId.empty()){err="License must be explicit; use UNKNOWN when unverified.";return false;}
 for(const auto& s:samples){
  if(s.sampleId.empty()||!safePath(s.relativePath)){err="Sample ID/path is invalid or escapes bank root.";return false;}
  if(!hashOk(s.sha256)){err="SHA-256 must contain 64 hexadecimal characters.";return false;}
  if(s.fileBytes==0||s.frames==0||!std::isfinite(s.sampleRate)||s.sampleRate<8000.0||s.sampleRate>384000.0||s.channels==0||s.channels>2){
   err="Sample metadata is invalid for current mono/stereo WAV playback.";return false;}
 }
 return true;
}
}