#include "BankManifest.h"
#include <iostream>
int main(){using namespace ppgu::sampler; BankManifest m; m.bankId="bank-1";m.displayName="Test";m.canonicalRoot="C:/PPGU/Test";m.license.licenseId="UNKNOWN";
 BankSampleRecord s;s.sampleId="sample-1";s.relativePath="Guitars/clean.wav";s.sha256=std::string(64,'a');s.fileBytes=4096;s.frames=2048;s.sampleRate=48000;s.channels=2;s.bitsPerSample=24;m.samples.push_back(s);
 std::string e;if(!m.validate(e)){std::cerr<<e;return 1;} BankManifest copy;
 if(!BankManifest::fromJson(m.toJson(),copy,e)||copy.samples.size()!=1||copy.license.redistributionAllowed)return 2;
 auto bad=m;bad.samples[0].relativePath="../escape.wav";if(bad.validate(e))return 3;
 bad=m;bad.samples[0].sha256="bad";if(bad.validate(e))return 4;
 bad=m;bad.samples.push_back(bad.samples[0]);if(bad.validate(e))return 6;
 auto negative=m.toJson(); auto* entries=negative.getDynamicObject()->getProperty("samples").getArray(); entries->getReference(0).getDynamicObject()->setProperty("fileBytes",-1);
 if(BankManifest::fromJson(negative,copy,e))return 7;
 auto json=m.toJson();json.getDynamicObject()->setProperty("schemaVersion",999);copy.bankId="preserve";
 if(BankManifest::fromJson(json,copy,e)||copy.bankId!="preserve")return 5;
 std::cout<<"BankManifest schema tests passed.\n";return 0;}