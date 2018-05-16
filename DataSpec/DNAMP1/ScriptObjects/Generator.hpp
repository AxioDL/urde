#ifndef _DNAMP1_GENERATOR_HPP_
#define _DNAMP1_GENERATOR_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct Generator : IScriptObject
{
    AT_DECL_DNA_YAML
    AT_DECL_DNAV
    String<-1> name;
    Value<atUint32> spawnCount;
    Value<bool> noReuseFollowers;
    Value<bool> noInheritXf;
    Value<atVec3f> offset;
    Value<bool> active;
    Value<float> minScale;
    Value<float> maxScale;
};
}

#endif
