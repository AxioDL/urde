#ifndef __URDE_CSTATEMANAGER_HPP__
#define __URDE_CSTATEMANAGER_HPP__

#include <memory>
#include <set>
#include "CBasics.hpp"
#include "World/ScriptObjectSupport.hpp"
#include "GameObjectLists.hpp"
#include "Camera/CCameraManager.hpp"
#include "Camera/CCameraFilter.hpp"
#include "CRandom16.hpp"
#include "zeus/CAABox.hpp"
#include "Weapon/CWeaponMgr.hpp"
#include "World/CAi.hpp"
#include "CToken.hpp"
#include "World/ScriptLoader.hpp"
#include "Input/CFinalInput.hpp"
#include "CSortedLists.hpp"
#include "World/CFluidPlaneManager.hpp"
#include "World/CEnvFxManager.hpp"
#include "World/CActorModelParticles.hpp"
#include "Input/CRumbleManager.hpp"
#include "Camera/CCameraShakeData.hpp"
#include "Graphics/Shaders/CColoredQuadFilter.hpp"

namespace urde
{
class CRelayTracker;
class CMapWorldInfo;
class CPlayerState;
class CWorldTransManager;
class CObjectList;
class CSortedListManager;
class CFluidPlaneManager;
class CEnvFxManager;
class CActorModelParticles;
class CTeamAiTypes;
class CRumbleManager;
class CActor;
class CLight;
class CDamageInfo;
class CMaterialFilter;
class CFinalInput;
class CPlayer;
class CWorld;
class CTexture;
class CWorldLayerState;
class CProjectedShadow;

namespace MP1
{
class CMFGameLoader;
}

struct SScriptObjectStream
{
    //CEntity* x0_obj;
    EScriptObjectType x0_type;
    u32 x4_position;
    u32 x8_length;
};

struct SOnScreenTex
{
    ResId x0_id = -1;
    zeus::CVector2i x4_origin;
    zeus::CVector2i xc_extent;
};

enum class EStateManagerTransition
{
    InGame,
    MapScreen,
    PauseGame,
    LogBook,
    SaveGame,
    MessageScreen
};

class CStateManager
{
    friend class MP1::CMFGameLoader;
public:
    enum class EGameState
    {
        Running,
        SoftPaused,
        Paused
    };

private:
    s16 x0_nextFreeIndex = 0;
    TUniqueId x8_idArr[1024] = {};

    /*
    std::unique_ptr<CObjectList> x80c_allObjs;
    std::unique_ptr<CActorList> x814_actorObjs;
    std::unique_ptr<CPhysicsActorList> x81c_physActorObjs;
    std::unique_ptr<CGameCameraList> x824_cameraObjs;
    std::unique_ptr<CGameLightList> x82c_lightObjs;
    std::unique_ptr<CListeningAiList> x834_listenAiObjs;
    std::unique_ptr<CAiWaypointList> x83c_aiWaypointObjs;
    std::unique_ptr<CPlatformAndDoorList> x844_platformAndDoorObjs;
     */
    std::unique_ptr<CObjectList> x808_objLists[8] =
    {std::make_unique<CObjectList>(EGameObjectList::All),
     std::make_unique<CActorList>(),
     std::make_unique<CPhysicsActorList>(),
     std::make_unique<CGameCameraList>(),
     std::make_unique<CGameLightList>(),
     std::make_unique<CListeningAiList>(),
     std::make_unique<CAiWaypointList>(),
     std::make_unique<CPlatformAndDoorList>()};

    std::unique_ptr<CPlayer> x84c_player;
    std::unique_ptr<CWorld> x850_world;

    /* Used to be a list of 32-element reserved_vectors */
    std::vector<TUniqueId> x854_objectGraveyard;

    struct CStateManagerContainer
    {
        CCameraManager x0_cameraManager;
        CSortedListManager x3c0_sortedListManager;
        CWeaponMgr xe3d8_weaponManager;
        CFluidPlaneManager xe3ec_fluidPlaneManager;
        CEnvFxManager xe510_envFxManager;
        CActorModelParticles xf168_actorModelParticles;
        CRumbleManager xf250_rumbleManager;
        u32 xf344_ = 0;
        rstl::reserved_vector<TUniqueId, 20> xf370_;
        rstl::reserved_vector<TUniqueId, 20> xf39c_renderLast;
    };
    std::unique_ptr<CStateManagerContainer> x86c_stateManagerContainer;
    CCameraManager* x870_cameraManager = nullptr;
    CSortedListManager* x874_sortedListManager = nullptr;
    CWeaponMgr* x878_weaponManager = nullptr;
    CFluidPlaneManager* x87c_fluidPlaneManager = nullptr;
    CEnvFxManager* x880_envFxManager = nullptr;
    CActorModelParticles* x884_actorModelParticles = nullptr;
    CRumbleManager* x88c_rumbleManager = nullptr;

    std::multimap<TEditorId, TUniqueId> x890_scriptIdMap;
    std::map<TEditorId, SScriptObjectStream> x8a4_loadedScriptObjects;

    std::shared_ptr<CPlayerState> x8b8_playerState;
    std::shared_ptr<CRelayTracker> x8bc_relayTracker;
    std::shared_ptr<CMapWorldInfo> x8c0_mapWorldInfo;
    std::shared_ptr<CWorldTransManager> x8c4_worldTransManager;
    std::shared_ptr<CWorldLayerState> x8c8_worldLayerState;

    TAreaId x8cc_nextAreaId = 0;
    TAreaId x8d0_prevAreaId = kInvalidAreaId;
    //u32 x8d0_extFrameIdx = 0;
    u32 x8d4_inputFrameIdx = 0;
    u32 x8d8_updateFrameIdx = 0;
    u32 x8dc_objectDrawToken = 0;

    std::vector<CLight> x8e0_dynamicLights;

    TLockedToken<CTexture> x8f0_shadowTex; /* DefaultShadow in MiscData */
    CRandom16 x8fc_random;
    CRandom16* x900_activeRandom = nullptr;
    EGameState x904_gameState = EGameState::Running;
    u32 x908_ = 0;
    FScriptLoader x90c_loaderFuncs[int(EScriptObjectType::ScriptObjectTypeMAX)] = {};

    bool xab0_worldLoaded = false;

    enum class EInitPhase
    {
        LoadWorld,
        LoadFirstArea,
        Done
    } xb3c_initPhase = EInitPhase::LoadWorld;

    std::set<std::string> xb40_uniqueInstanceNames;

    CFinalInput xb54_finalInput;
    CCameraFilterPassPoly xb84_camFilterPasses[9];
    CCameraBlurPass xd14_camBlurPasses[9];

    s32 xe60_ = -1;
    zeus::CVector3f xe64_;

    TUniqueId xe70_ = kInvalidUniqueId;
    zeus::CVector3f xe74_ = {0.f, 1.f, 1.f};

    s32 xe80_ = 2;
    TUniqueId xe84_ = kInvalidUniqueId;

    union
    {
        struct
        {
            bool xe86_24_ : 1;
            bool xe86_25_ : 1;
            bool xe86_26_ : 1;
            bool xe86_27_ : 1;
            bool xe86_28_ : 1;
            bool xe86_29_ : 1;
        };
        u16 _dummy = 0;
    };

    u32 xeec_hintIdx = -1;
    u32 xef0_hintPeriods = 0;

    SOnScreenTex xef4_pendingScreenTex;
    ResId xf08_pauseHudMessage = -1;
    float xf0c_escapeTimer = 0.f;
    float xf10_escapeTotalTime = 0.f;
    float xf14_curTimeMod900 = 0.f;
    TUniqueId xf18_bossId = kInvalidUniqueId;
    float xf1c_totalBossEnergy = 0.f;
    u32 xf20_bossStringIdx = 0;
    float xf24_thermColdScale1 = 0.f;
    float xf28_thermColdScale2 = 0.f;
    zeus::CVector2f xf2c_viewportScale = {1.f, 1.f};
    u32 xf34_particleFlags = 2;
    TUniqueId xf38_skipCineSpecialFunc = kInvalidUniqueId;
    std::list<u32> xf3c_;
    u32 xf50_ = 0;
    std::list<u32> xf54_;
    u32 xf68_ = 0;
    TUniqueId xf6c_playerActor = kInvalidUniqueId;
    u32 xf70_ = 0;

    TUniqueId xf74_lastTrigger = kInvalidUniqueId;
    TUniqueId xf76_lastRelay = kInvalidUniqueId;

    float xf78_hudMessageTime = 0.f;
    CProjectedShadow* xf7c_projectedShadow = nullptr;
    u32 xf80_hudMessageFrameCount = 0;
    ResId xf84_ = -1;
    ResId xf88_ = -1;
    float xf8c_ = 0.f;
    EStateManagerTransition xf90_deferredTransition = EStateManagerTransition::InGame;

    union
    {
        struct
        {
            bool xf94_24_ : 1;
            bool xf94_25_quitGame : 1;
            bool xf94_26_generatingObject : 1;
            bool xf94_27_inMapScreen : 1;
            bool xf94_28_inSaveUI : 1;
            bool xf94_29_ : 1;
            bool xf94_30_fullThreat : 1;
        };
        u32 xf94_ = 0;
    };

    CColoredQuadFilter m_deathWhiteout = { EFilterType::Add };
    CColoredQuadFilter m_escapeWhiteout = { EFilterType::Add };

    void UpdateThermalVisor();
    static void RendererDrawCallback(const void*, const void*, int);

public:
    CStateManager(const std::weak_ptr<CRelayTracker>&,
                  const std::weak_ptr<CMapWorldInfo>&,
                  const std::weak_ptr<CPlayerState>&,
                  const std::weak_ptr<CWorldTransManager>&,
                  const std::weak_ptr<CWorldLayerState>&);

    u32 GetInputFrameIdx() const { return x8d4_inputFrameIdx; }
    bool RenderLast(TUniqueId);
    void AddDrawableActorPlane(const CActor& actor, const zeus::CPlane&, const zeus::CAABox& aabb) const;
    void AddDrawableActor(const CActor& actor, const zeus::CVector3f& vec, const zeus::CAABox& aabb) const;
    bool SpecialSkipCinematic();
    TAreaId GetVisAreaId() const;
    s32 GetWeaponIdCount(TUniqueId, EWeaponType);
    void RemoveWeaponId(TUniqueId, EWeaponType);
    void AddWeaponId(TUniqueId, EWeaponType);
    void UpdateEscapeSequenceTimer(float);
    float GetEscapeSequenceTimer() const { return xf0c_escapeTimer; }
    void ResetEscapeSequenceTimer(float);
    void SetupParticleHook(const CActor& actor) const;
    void MurderScriptInstanceNames();
    std::string HashInstanceName(CInputStream& in);
    void SetActorAreaId(CActor& actor, TAreaId);
    void TouchSky() const;
    void TouchPlayerActor();
    void DrawSpaceWarp(const zeus::CVector3f&, float) const;
    void DrawReflection(const zeus::CVector3f&);
    static void ReflectionDrawer(void*, const zeus::CVector3f&);
    void CacheReflection();
    bool CanCreateProjectile(TUniqueId, EWeaponType, int) const;
    const std::vector<CLight>& GetDynamicLightList() const { return x8e0_dynamicLights; }
    void BuildDynamicLightListForWorld();
    void DrawDebugStuff() const;
    void RenderCamerasAndAreaLights() const;
    void DrawE3DeathEffect() const;
    void DrawAdditionalFilters() const;
    zeus::CFrustum SetupViewForDraw(const SViewport& vp) const;
    void ResetViewAfterDraw(const SViewport& backupViewport, const zeus::CTransform& backupViewMatrix) const;
    void DrawWorld() const;
    void SetupFogForArea(const CGameArea& area) const;
    bool SetupFogForDraw() const;
    void PreRender();
    bool GetVisSetForArea(TAreaId, TAreaId, CPVSVisSet& setOut) const;
    void RecursiveDrawTree(TUniqueId) const;
    void SendScriptMsg(CEntity* dest, TUniqueId src, EScriptObjectMessage msg);
    void SendScriptMsg(TUniqueId dest, TUniqueId src, EScriptObjectMessage msg);
    void SendScriptMsg(TUniqueId src, TEditorId dest,
                       EScriptObjectMessage msg, EScriptObjectState state);
    void SendScriptMsgAlways(TUniqueId dest, TUniqueId src, EScriptObjectMessage);
    void FreeScriptObjects(TAreaId);
    void FreeScriptObject(TUniqueId);
    std::pair<const SScriptObjectStream*, TEditorId> GetBuildForScript(TEditorId) const;
    TEditorId GetEditorIdForUniqueId(TUniqueId) const;
    TUniqueId GetIdForScript(TEditorId) const;
    std::pair<std::multimap<TEditorId, TUniqueId>::const_iterator,
              std::multimap<TEditorId, TUniqueId>::const_iterator>
    GetIdListForScript(TEditorId) const;
    void LoadScriptObjects(TAreaId, CInputStream& in, std::vector<TEditorId>& idsOut);
    void InitializeScriptObjects(const std::vector<TEditorId>& objIds);
    std::pair<TEditorId, TUniqueId> LoadScriptObject(TAreaId, EScriptObjectType, u32, CInputStream& in);
    std::pair<TEditorId, TUniqueId> GenerateObject(TEditorId);
    void InitScriptObjects(const std::vector<TEditorId>& ids);
    void InformListeners(const zeus::CVector3f&, EListenNoiseType);
    void ApplyKnockBack(CActor& actor, const CDamageInfo& info,
                        const CDamageVulnerability&, const zeus::CVector3f&, float);
    void KnockBackPlayer(CPlayer& player, const zeus::CVector3f& pos, float power, float resistance);
    void ApplyDamageToWorld(TUniqueId, const CActor&, const zeus::CVector3f&,
                            const CDamageInfo& info, const CMaterialFilter&);
    void ProcessRadiusDamage(const CActor&, CActor&, TUniqueId senderId,
                             const CDamageInfo& info, const CMaterialFilter&);
    void ApplyRadiusDamage(const CActor&, const zeus::CVector3f&, CActor&,
                           const CDamageInfo& info);
    bool TestRayDamage(const zeus::CVector3f& pos, const CActor& damagee,
                       const rstl::reserved_vector<TUniqueId, 1024>& nearList);
    bool RayCollideWorld(const zeus::CVector3f& pos, const zeus::CVector3f& damageeCenter,
                         const rstl::reserved_vector<TUniqueId, 1024>& nearList,
                         const CMaterialFilter& filter, const CActor& damagee);
    bool RayCollideWorldInternal(const zeus::CVector3f& pos, const zeus::CVector3f& damageeCenter,
                                 const CMaterialFilter& filter,
                                 const rstl::reserved_vector<TUniqueId, 1024>& nearList,
                                 const CActor* damagee);
    bool MultiRayCollideWorld(const zeus::CMRay& ray, const CMaterialFilter& filter);
    void TestBombHittingWater(const CActor& damager, const zeus::CVector3f& pos, CActor& damagee);
    bool ApplyLocalDamage(const zeus::CVector3f&, const zeus::CVector3f&, CActor&, float,
                          const CWeaponMode&);
    bool ApplyDamage(TUniqueId, TUniqueId, TUniqueId, const CDamageInfo& info,
                     const CMaterialFilter&, const zeus::CVector3f&);
    void UpdateAreaSounds();
    void FrameEnd();
    void ProcessPlayerInput();
    void SetGameState(EGameState state);
    EGameState GetGameState() const { return x904_gameState; }
    void ProcessInput(const CFinalInput& input);
    void Update(float dt);
    void UpdateGameState();
    void UpdateHintState(float dt);
    void PreThinkEffects(float dt);
    void MovePlatforms(float dt);
    void MoveDoors(float dt);
    void CrossTouchActors();
    void ThinkEffectsAndActors(float dt);
    void UpdatePlayer(float dt);
    void ShowPausedHUDMemo(ResId strg, float time);
    void ClearGraveyard();
    void FrameBegin(s32 frameCount);
    void InitializeState(ResId mlvlId, TAreaId aid, ResId mreaId);
    void CreateStandardGameObjects();
    const std::unique_ptr<CObjectList>& GetObjectList() const { return x808_objLists[0]; }
    CObjectList* ObjectListById(EGameObjectList type);
    const CObjectList* GetObjectListById(EGameObjectList type) const;
    void RemoveObject(TUniqueId);
    void UpdateRoomAcoustics(TAreaId);
    TAreaId GetNextAreaId() const { return x8cc_nextAreaId; }
    void SetCurrentAreaId(TAreaId);
    void DeleteObjectRequest(TUniqueId);
    CEntity* ObjectById(TUniqueId uid);
    const CEntity* GetObjectById(TUniqueId uid) const;
    void AreaUnloaded(TAreaId);
    void PrepareAreaUnload(TAreaId);
    void AreaLoaded(TAreaId);
    void BuildNearList(rstl::reserved_vector<TUniqueId, 1024>& listOut,
                       const zeus::CVector3f&, const zeus::CVector3f&, float,
                       const CMaterialFilter&, const CActor*) const;
    void BuildColliderList(rstl::reserved_vector<TUniqueId, 1024>& listOut,
                           const CActor&, const zeus::CAABox&) const;
    void BuildNearList(rstl::reserved_vector<TUniqueId, 1024>& listOut,
                       const zeus::CAABox&, const CMaterialFilter&, const CActor*) const;
    void UpdateActorInSortedLists(CActor&);
    void UpdateSortedLists();
    std::experimental::optional<zeus::CAABox> CalculateObjectBounds(const CActor&);
    void AddObject(CEntity&);
    void AddObject(CEntity*);
    CRayCastResult RayStaticIntersection(const zeus::CVector3f& pos, const zeus::CVector3f& dir, float length,
                                         const CMaterialFilter& filter) const;
    CRayCastResult RayWorldIntersection(TUniqueId& idOut, const zeus::CVector3f& pos,
                                        const zeus::CVector3f& dir, float length, const CMaterialFilter& filter,
                                        const rstl::reserved_vector<TUniqueId, 1024>& list) const;
    void UpdateObjectInLists(CEntity&);
    TUniqueId AllocateUniqueId();
    void DeferStateTransition(EStateManagerTransition t);
    EStateManagerTransition GetDeferredStateTransition() const { return xf90_deferredTransition; }
    bool CanShowMapScreen() const;
    TUniqueId GetSkipCinematicSpecialFunction() const { return xf38_skipCineSpecialFunc; }
    void SetSkipCinematicSpecialFunction(TUniqueId id) { xf38_skipCineSpecialFunc = id; }
    float GetHUDMessageTime() const { return xf78_hudMessageTime; }
    ResId GetPauseHUDMessage() const { return xf08_pauseHudMessage; }
    void IncrementHUDMessageFrameCounter() { ++xf80_hudMessageFrameCount; }
    bool ShouldQuitGame() const { return xf94_25_quitGame; }
    void SetInSaveUI(bool b) { xf94_28_inSaveUI = b; }
    void SetInMapScreen(bool b) { xf94_27_inMapScreen = b; }
    bool GetInMapScreen() const { return xf94_27_inMapScreen; }
    bool IsFullThreat() const { return xf94_30_fullThreat; }

    const std::shared_ptr<CPlayerState>& GetPlayerState() const {return x8b8_playerState;}
    CRandom16* GetActiveRandom() {return x900_activeRandom;}
    void SetActiveRandomToDefault() { x900_activeRandom = &x8fc_random; }
    void ClearActiveRandom() { x900_activeRandom = nullptr; }
    CRumbleManager& GetRumbleManager() {return *x88c_rumbleManager;}
    CCameraFilterPassPoly& GetCameraFilterPass(int idx) {return xb84_camFilterPasses[idx];}
    CCameraBlurPass& GetCameraBlurPass(int idx) {return xd14_camBlurPasses[idx];}

    CEnvFxManager* GetEnvFxManager() { return x880_envFxManager; }
    CWorld* WorldNC() {return x850_world.get();}
    const CWorld* GetWorld() const { return x850_world.get(); }
    CRelayTracker* GetRelayTracker() { return x8bc_relayTracker.get(); }
    CCameraManager* GetCameraManager() const { return x870_cameraManager; }

    const std::shared_ptr<CMapWorldInfo>& MapWorldInfo() const { return x8c0_mapWorldInfo; }
    const std::shared_ptr<CWorldLayerState>& LayerState() const { return x8c8_worldLayerState; }

    CPlayer& GetPlayer() const { return *x84c_player; }
    CPlayer* Player() const { return x84c_player.get(); }

    CObjectList& GetAllObjectList() const { return *x808_objLists[0]; }
    CActorList& GetActorObjectList() const { return static_cast<CActorList&>(*x808_objLists[1]); }
    CPhysicsActorList& GetPhysicsActorObjectList() const { return static_cast<CPhysicsActorList&>(*x808_objLists[2]); }
    CGameCameraList& GetCameraObjectList() const { return static_cast<CGameCameraList&>(*x808_objLists[3]); }
    CGameLightList& GetLightObjectList() const { return static_cast<CGameLightList&>(*x808_objLists[4]); }
    CListeningAiList& GetListeningAiObjectList() const { return static_cast<CListeningAiList&>(*x808_objLists[5]); }
    CAiWaypointList& GetAiWaypointObjectList() const { return static_cast<CAiWaypointList&>(*x808_objLists[6]); }
    CPlatformAndDoorList& GetPlatformAndDoorObjectList() const { return static_cast<CPlatformAndDoorList&>(*x808_objLists[7]); }
    std::pair<u32, u32> CalculateScanCompletionRate() const;
    void SetLastTriggerId(TUniqueId uid) { xf74_lastTrigger = uid; }
    TUniqueId GetLastTriggerId() const { return xf74_lastTrigger; }
    void SetLastRelayId(TUniqueId uid) { xf76_lastRelay = uid; }
    TUniqueId* GetLastRelayIdPtr() { return &xf76_lastRelay; }
    TUniqueId GetLastRelayId() const { return xf76_lastRelay; }
    bool GetIsGeneratingObject() const { return xf94_26_generatingObject; }
    void SetIsGeneratingObject(bool gen) { xf94_26_generatingObject = gen; }
    u32 GetParticleFlags() const { return xf34_particleFlags; }
    const CFinalInput& GetFinalInput() const { return xb54_finalInput; }
    void SetBossParams(TUniqueId bossId, float maxEnergy, u32 stringIdx);
    TUniqueId GetBossId() const { return xf18_bossId; }
    float GetTotalBossEnergy() const { return xf1c_totalBossEnergy; }
    u32 GetBossStringIdx() const { return xf20_bossStringIdx; }
    const SOnScreenTex& GetPendingScreenTex() const { return xef4_pendingScreenTex; }
    void SetViewportScale(const zeus::CVector2f& scale) { xf2c_viewportScale = scale; }

    static float g_EscapeShakeCountdown;
    static bool g_EscapeShakeCountdownInit;
};
}

#endif
