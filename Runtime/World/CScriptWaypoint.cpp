#include "Runtime/World/CScriptWaypoint.hpp"

#include "Runtime/CStateManager.hpp"
#include "Runtime/World/CActorParameters.hpp"

#include "TCastTo.hpp" // Generated file, do not modify include path

namespace metaforce {

CScriptWaypoint::CScriptWaypoint(TUniqueId uid, std::string_view name, const CEntityInfo& info,
                                 const zeus::CTransform& xf, bool active, float speed, float pause,
                                 u32 patternTranslate, u32 patternOrient, u32 patternFit, u32 behaviour,
                                 u32 behaviourOrient, u32 behaviourModifiers, u32 animation)
: CActor(uid, active, name, info, xf, CModelData(), CMaterialList(), CActorParameters::None(), kInvalidUniqueId)
, xe8_speed(speed)
, xec_animation(animation)
, xf0_pause(pause)
, xf4_patternTranslate(patternTranslate)
, xf5_patternOrient(patternOrient)
, xf6_patternFit(patternFit)
, xf7_behaviour(behaviour)
, xf8_behaviourOrient(behaviourOrient)
, xfa_behaviourModifiers(behaviourModifiers) {
  SetUseInSortedLists(false);
  SetCallTouch(false);
}

void CScriptWaypoint::Accept(IVisitor& visitor) { visitor.Visit(this); }

void CScriptWaypoint::AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId sender, CStateManager& mgr) {
  CActor::AcceptScriptMsg(msg, sender, mgr);
  if (GetActive() && msg == EScriptObjectMessage::Arrived)
    SendScriptMsgs(EScriptObjectState::Arrived, mgr, EScriptObjectMessage::None);
}

void CScriptWaypoint::AddToRenderer(const zeus::CFrustum&, CStateManager&) {
  // Empty
}

TUniqueId CScriptWaypoint::FollowWaypoint(CStateManager& mgr) const {
  for (const SConnection& conn : x20_conns) {
    if (conn.x0_state == EScriptObjectState::Arrived && conn.x4_msg == EScriptObjectMessage::Follow) {
      return mgr.GetIdForScript(conn.x8_objId);
    }
  }
  return kInvalidUniqueId;
}

TUniqueId CScriptWaypoint::NextWaypoint(CStateManager& mgr) const {
  rstl::reserved_vector<TUniqueId, 10> ids;
  for (const SConnection& conn : x20_conns) {
    if (conn.x0_state == EScriptObjectState::Arrived && conn.x4_msg == EScriptObjectMessage::Next) {
      const TUniqueId id = mgr.GetIdForScript(conn.x8_objId);
      if (id != kInvalidUniqueId) {
        if (const TCastToConstPtr<CScriptWaypoint> wp = mgr.GetObjectById(id)) {
          if (wp->GetActive()) {
            ids.push_back(wp->GetUniqueId());
          }
        }
      }
    }
  }

  if (ids.empty()) {
    return kInvalidUniqueId;
  }

  return ids[int(mgr.GetActiveRandom()->Float() * ids.size() * 0.99f)];
}

} // namespace metaforce
