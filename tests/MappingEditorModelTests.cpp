#include "MappingEditorModel.h"
#include <cassert>
#include <iostream>

int main()
{
    using namespace ppgu::sampler;
    MappingEditorModel model;
    std::vector<MappingRegion> regions(3);
    for (std::size_t i=0;i<3;++i)
    {
        regions[i].id=i+1;
        regions[i].region.samplePath="sample"+std::to_string(i)+".wav";
        regions[i].region.lowKey=48; regions[i].region.highKey=60;
        regions[i].region.rootKey=54;
        regions[i].region.lowVelocity=1; regions[i].region.highVelocity=127;
        regions[i].roundRobinGroup=7;
    }
    std::string error;
    assert(model.replace(regions,error));
    auto a=model.select(54,100), b=model.select(54,100), c=model.select(54,100), d=model.select(54,100);
    assert(a && b && c && d);
    assert(a->regionId==1 && b->regionId==2 && c->regionId==3 && d->regionId==1);
    assert(model.setKeyRange(1,50,58,54,error));
    assert(model.setVelocityRange(1,1,63,error));
    assert(model.setRoundRobin(1,0,error));
    assert(!model.setKeyRange(1,70,60,65,error));
    assert(!model.select(49,80));
    assert(model.select(54,50));
    model.resetRoundRobin();
    std::cout << "Mapping editor model tests passed\n";
}
