#include "ChordRecognizer.h"
#include <algorithm>
#include <array>
#include <set>

namespace ppgu::music
{
namespace
{
struct Template { ChordQuality quality; const char* suffix; std::array<int,4> intervals; int count; };
constexpr Template templates[] = {
 {ChordQuality::Major,"",{0,4,7,0},3},
 {ChordQuality::Minor,"m",{0,3,7,0},3},
 {ChordQuality::Power,"5",{0,7,0,0},2},
 {ChordQuality::Sus2,"sus2",{0,2,7,0},3},
 {ChordQuality::Sus4,"sus4",{0,5,7,0},3},
 {ChordQuality::Diminished,"dim",{0,3,6,0},3},
 {ChordQuality::Augmented,"aug",{0,4,8,0},3},
 {ChordQuality::Major7,"maj7",{0,4,7,11},4},
 {ChordQuality::Minor7,"m7",{0,3,7,10},4},
 {ChordQuality::Dominant7,"7",{0,4,7,10},4}
};
}
const char* ChordRecognizer::noteName(int pc) noexcept
{
 static constexpr const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
 return names[((pc%12)+12)%12];
}
const char* ChordRecognizer::qualityName(ChordQuality q) noexcept
{
 switch(q) {
 case ChordQuality::Note:return "NOTE"; case ChordQuality::Major:return "MAJOR";
 case ChordQuality::Minor:return "MINOR"; case ChordQuality::Power:return "POWER CHORD";
 case ChordQuality::Sus2: case ChordQuality::Sus4:return "SUSPENDED";
 case ChordQuality::Diminished:return "DIMINISHED"; case ChordQuality::Augmented:return "AUGMENTED";
 case ChordQuality::Major7:return "MAJOR 7"; case ChordQuality::Minor7:return "MINOR 7";
 case ChordQuality::Dominant7:return "DOMINANT 7"; default:return "AMBIGUOUS";
 }
}
ChordAnalysis ChordRecognizer::analyze(const std::vector<int>& input)
{
 ChordAnalysis out;
 std::set<int> pcs;
 std::set<int> uniqueMidi;
 for(int n:input) if(n>=0 && n<=127) { uniqueMidi.insert(n); pcs.insert(n%12); }
 out.midiNotes.assign(uniqueMidi.begin(),uniqueMidi.end());
 out.pitchClasses.assign(pcs.begin(),pcs.end());
 if(pcs.empty()) return out;
 out.valid=true; out.bassPitchClass=(*uniqueMidi.begin())%12;
 if(pcs.size()==1) {
  out.rootPitchClass=*pcs.begin(); out.quality=ChordQuality::Note;
  out.symbol=noteName(out.rootPitchClass);
  const int midi=*uniqueMidi.begin();
  const int octave=(midi/12)-1;
  out.displayText=out.symbol+std::to_string(octave);\n  out.qualityLabel="NOTE"; return out;
 }
 struct Candidate { int root; const Template* t; int missing; int extra; };
 std::vector<Candidate> candidates;
 for(int root:pcs) for(const auto& t:templates) {
  int matched=0, missing=0, extra=0;
  std::set<int> expected;
  for(int i=0;i<t.count;++i) expected.insert((root+t.intervals[static_cast<size_t>(i)])%12);
  for(int pc:expected) if(pcs.count(pc)) ++matched; else ++missing;
  for(int pc:pcs) if(!expected.count(pc)) ++extra;
  // Exact templates first; tolerate one omitted chord tone only for triads/sevenths.
  if(extra==0 && missing==0) candidates.push_back({root,&t,0,0});
  else if(extra==0 && missing==1 && t.count>=3) candidates.push_back({root,&t,1,0});
 }
 if(candidates.empty()) {
  out.ambiguous=true; out.quality=ChordQuality::Unknown; out.qualityLabel="AMBIGUOUS";
  for(int pc:pcs) { if(!out.symbol.empty()) out.symbol+=" + "; out.symbol+=noteName(pc); }
  return out;
 }
 std::sort(candidates.begin(),candidates.end(),[](const Candidate&a,const Candidate&b){
  if(a.missing!=b.missing) return a.missing<b.missing;
  return a.t->count>b.t->count;
 });
 const auto best=candidates.front();
 // If equally supported interpretations disagree, do not guess.
 for(size_t i=1;i<candidates.size();++i) {
  if(candidates[i].missing!=best.missing || candidates[i].t->count!=best.t->count) break;
  if(candidates[i].root!=best.root || candidates[i].t->quality!=best.t->quality) {
   out.ambiguous=true; out.quality=ChordQuality::Unknown; out.qualityLabel="AMBIGUOUS";
   for(int pc:pcs) { if(!out.symbol.empty()) out.symbol+=" + "; out.symbol+=noteName(pc); }
   return out;
  }
 }
 out.rootPitchClass=best.root; out.quality=best.t->quality;
 out.symbol=std::string(noteName(best.root))+best.t->suffix;
 if(out.bassPitchClass!=best.root) out.symbol+="/"+noteName(out.bassPitchClass);
 out.qualityLabel=qualityName(best.t->quality);
 return out;
}
}
