#include "ChordRecognizer.h"
#include <cassert>
#include <iostream>
using ppgu::music::ChordRecognizer;
int main(){
 auto check=[](std::vector<int> n,const char* s,const char* q){auto a=ChordRecognizer::analyze(n);assert(a.valid&&!a.ambiguous);assert(a.symbol==s);assert(a.qualityLabel==q);};
 check({60},"C","NOTE");
 auto single=ChordRecognizer::analyze({60}); assert(single.displayText=="C4");
 auto sharp=ChordRecognizer::analyze({61}); assert(sharp.displayText=="C#4");
 auto octaveNote=ChordRecognizer::analyze({72}); assert(octaveNote.displayText=="C5");
 check({60,64,67},"C","MAJOR");
 check({60,63,67},"Cm","MINOR");
 check({57,60,64},"Am","MINOR");
 check({55,59,62},"G","MAJOR");
 check({60,67},"C5","POWER CHORD");
 check({60,65,67},"Csus4","SUSPENDED");
 check({60,63,66},"Cdim","DIMINISHED");
 check({60,64,68},"Caug","AUGMENTED");
 check({60,64,67,71},"Cmaj7","MAJOR 7");
 check({60,63,67,70},"Cm7","MINOR 7");
 check({60,64,67,70},"C7","DOMINANT 7");
 check({64,67,72},"C/E","MAJOR");
 auto a=ChordRecognizer::analyze({60,61,62,65}); assert(a.valid&&a.ambiguous);
 auto octave=ChordRecognizer::analyze({48,60,64,67}); assert(octave.symbol=="C");
 std::cout<<"ChordRecognizer tests passed\n";
}
