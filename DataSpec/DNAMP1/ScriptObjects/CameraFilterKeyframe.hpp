#ifndef _DNAMP1_CAMERAFILTERKEYFRAME_HPP_
#define _DNAMP1_CAMERAFILTERKEYFRAME_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct CameraFilterKeyframe : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<bool> active;
    Value<atUint32> type;
    Value<atUint32> shape;
    Value<atUint32> filterIdx;
    Value<atUint32> unk;
    DNAColor color;
    Value<float> timeIn;
    Value<float> timeOut;
    UniqueID32 texture;

    void nameIDs(PAKRouter<PAKBridge>& pakRouter) const
    {
        if (texture)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture);
            ent->name = name + "_texture";
        }
    }

    void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut) const
    {
        g_curSpec->flattenDependencies(texture, pathsOut);
    }
};
}
}

#endif
