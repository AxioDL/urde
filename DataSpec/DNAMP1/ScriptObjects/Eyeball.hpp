#pragma once

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1 {
struct Eyeball : IScriptObject {
  AT_DECL_DNA_YAMLV
  String<-1> name;
  Value<atUint32> flavor;
  Value<atVec3f> location;
  Value<atVec3f> orientation;
  Value<atVec3f> scale;
  PatternedInfo patternedInfo;
  ActorParameters actorParameters;
  Value<float> attackDelay;
  Value<float> attackStartTime;
  UniqueID32 wpsc;
  DamageInfo damageInfo;
  UniqueID32 beamContactFxId;
  UniqueID32 beamPulseFxId;
  UniqueID32 beamTextureId;
  UniqueID32 beamGlowTextureId;
  Value<atUint32> anim0;
  Value<atUint32> anim1; // always ff
  Value<atUint32> anim2; // always ff
  Value<atUint32> anim3; // always ff
  Value<atUint32> beamSfx;
  Value<bool> attackDisabled;

  void addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter, CharacterAssociations<UniqueID32>& charAssoc) const override {
    actorParameters.addCMDLRigPairs(pakRouter, charAssoc, patternedInfo.animationParameters);
  }

  void nameIDs(PAKRouter<PAKBridge>& pakRouter) const override {
    if (wpsc.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc);
      ent->name = name + "_wpsc";
    }
    if (beamContactFxId.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(beamContactFxId);
      ent->name = name + "_part1";
    }
    if (beamPulseFxId.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(beamPulseFxId);
      ent->name = name + "_part2";
    }
    if (beamTextureId.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(beamTextureId);
      ent->name = name + "_tex1";
    }
    if (beamGlowTextureId.isValid()) {
      PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(beamGlowTextureId);
      ent->name = name + "_tex2";
    }
    patternedInfo.nameIDs(pakRouter, name + "_patterned");
    actorParameters.nameIDs(pakRouter, name + "_actp");
  }

  void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut,
                          std::vector<hecl::ProjectPath>& lazyOut) const override {
    g_curSpec->flattenDependencies(wpsc, pathsOut);
    g_curSpec->flattenDependencies(beamContactFxId, pathsOut);
    g_curSpec->flattenDependencies(beamPulseFxId, pathsOut);
    g_curSpec->flattenDependencies(beamTextureId, pathsOut);
    g_curSpec->flattenDependencies(beamGlowTextureId, pathsOut);
    patternedInfo.depIDs(pathsOut);
    actorParameters.depIDs(pathsOut, lazyOut);
  }

  void gatherScans(std::vector<Scan>& scansOut) const override { actorParameters.scanIDs(scansOut); }
};
} // namespace DataSpec::DNAMP1
