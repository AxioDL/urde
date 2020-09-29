#pragma once

#include "Runtime/Weapon/CEnergyProjectile.hpp"

namespace urde::MP1 {

class CFlaahgraProjectile : public CEnergyProjectile {
  bool x3d8_bigStrike;

public:
  CFlaahgraProjectile(bool bigStrike, const TToken<CWeaponDescription>& desc, const zeus::CTransform& xf,
                      const CDamageInfo& damage, TUniqueId uid, TAreaId aid, TUniqueId owner);

  void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId sender, CStateManager &mgr) override;
};

} // namespace urde::MP1
