#pragma once

#include <string>
#include <vector>
#include <set>

#include "Runtime/RetroTypes.hpp"
#include "Runtime/World/CEntityInfo.hpp"
#include "Runtime/World/ScriptObjectSupport.hpp"

#ifndef ENABLE_IMGUI
#define ENABLE_IMGUI 1
#endif

#if ENABLE_IMGUI
#define IMGUI_ENTITY_PROTOTYPES                                                                                        \
  std::string_view ImGuiType() override;                                                                               \
  void ImGuiInspect() override;
#else
#define IMGUI_ENTITY_PROTOTYPES
#endif

#define DEFINE_ENTITY IMGUI_ENTITY_PROTOTYPES

namespace metaforce {
class CStateManager;
class IVisitor;

class CEntity {
  friend class CStateManager;
  friend class CObjectList;
  friend class ImGuiConsole;

protected:
  TAreaId x4_areaId;
  TUniqueId x8_uid;
  TEditorId xc_editorId;
  std::string x10_name;
  std::vector<SConnection> x20_conns;
  bool x30_24_active : 1;
  bool x30_25_inGraveyard : 1 = false;
  bool x30_26_scriptingBlocked : 1 = false;
  bool x30_27_inUse : 1;

  // Used in ImGuiConsole
  bool m_debugSelected = false;
  bool m_debugHovered = false;
  const std::set<SConnection>* m_incomingConnections = nullptr;

public:
  static const std::vector<SConnection> NullConnectionList;
  virtual ~CEntity() = default;
  CEntity(TUniqueId uid, const CEntityInfo& info, bool active, std::string_view name);
  virtual void Accept(IVisitor& visitor) = 0;
  virtual void PreThink(float dt, CStateManager& mgr) {}
  virtual void Think(float dt, CStateManager& mgr) {}
  virtual void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId objId, CStateManager& stateMgr);
  virtual void SetActive(bool active) { x30_24_active = active; }

  // Debugging utilities
  virtual std::string_view ImGuiType();
  virtual void ImGuiInspect();

  bool GetActive() const { return x30_24_active; }
  void ToggleActive() { x30_24_active ^= 1; }

  bool IsInGraveyard() const { return x30_25_inGraveyard; }
  void SetIsInGraveyard(bool in) { x30_25_inGraveyard = in; }
  bool IsScriptingBlocked() const { return x30_26_scriptingBlocked; }
  void SetIsScriptingBlocked(bool blocked) { x30_26_scriptingBlocked = blocked; }
  bool IsInUse() const { return x30_27_inUse; }

  TAreaId GetAreaId() const {
    if (x30_27_inUse)
      return x4_areaId;
    return kInvalidAreaId;
  }
  TAreaId GetAreaIdAlways() const { return x4_areaId; }
  TUniqueId GetUniqueId() const { return x8_uid; }
  TEditorId GetEditorId() const { return xc_editorId; }
  void SendScriptMsgs(EScriptObjectState state, CStateManager& stateMgr, EScriptObjectMessage msg);

  std::vector<SConnection>& GetConnectionList() { return x20_conns; }
  const std::vector<SConnection>& GetConnectionList() const { return x20_conns; }

  std::string_view GetName() const { return x10_name; }
  void SetIncomingConnectionList(const std::set<SConnection>* conns) { m_incomingConnections = conns; }
};

} // namespace metaforce
