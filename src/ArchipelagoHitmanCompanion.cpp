#include "ArchipelagoHitmanCompanion.h"

#include <Logging.h>
#include <IconsMaterialDesign.h>
#include <Globals.h>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZScene.h>
#include <Glacier/ZHitman5.h>
#include <Glacier/ZActor.h> 
#include <Glacier/ZItem.h>
#include <Glacier/SExternalReferences.h>
#include <Glacier/ZModule.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <apclient.hpp>

static APClient* g_APClient = NULL;
static std::atomic<bool> g_APConnected = false;

void ArchipelagoHitmanCompanion::ConnectToArchipelago()
{
    if (g_APClient)
        return; // already connected or connecting

    bool error = false;
    bool roomInfo = false;
    bool roomUpdate = false;

    Logger::Info("Starting client for {}...", m_APServerAddressInput);

    g_APClient = new APClient("", m_GameName, m_APServerAddressInput);

    g_APClient->set_socket_connected_handler([this]()
        {
            Logger::Info("Connected to Archipelago server!");
            g_APConnected = true;
        });

    g_APClient->set_room_info_handler([&roomInfo, this]() {
        roomInfo = true;
        g_APClient->ConnectSlot(m_APSlotNameInput, m_APPasswordInput, 0, { "DeathLink" }, { 0, 6, 5 });
        });

    g_APClient->set_room_update_handler([&roomUpdate]() {
        roomUpdate = true;
        });

    g_APClient->set_socket_error_handler([&error](const std::string& msg) {
        Logger::Error("Error making Archipelago connection {}", msg);
        error = true;
        });

    g_APClient->set_socket_disconnected_handler([]()
        {
            g_APConnected = false;
        });

    g_APClient->set_bounced_handler([&](nlohmann::json packet) {
        std::list<std::string> tags = packet["tags"];

        bool deathlink = (std::find(tags.begin(), tags.end(), "DeathLink") != tags.end());

        if (deathlink) {
            auto data = packet["data"];
            std::string cause = "";
            if (data.contains("cause")) {
                cause = data["cause"];
            }
            std::string source = "";
            if (data.contains("source")) {
                source = data["source"];
            }

            double timestamp = data["time"];

            Logger::Info("Received DeathLink from {} with cause '{}'", source, cause);
			m_DeathFromDeathLink = true;
			KillHitman();
        }

        });
}

void DisconnectFromArchipelago()
{
    if (!g_APClient)
        return;
    g_APConnected = false;
}

void ArchipelagoHitmanCompanion::KillHitman()
{
    auto localHitman = SDK()->GetLocalPlayer();

    if(localHitman.m_pInterfaceRef){
        const ZRepositoryID p_RepositoryId = ZRepositoryID("af8a7b6c-692c-4a76-b9bc-2b91ce32bcbc");

        const auto s_Scene = Globals::Hitman5Module->m_pEntitySceneContext->m_pScene;

        const auto s_ID = ResId<"[modules:/zitemspawner.class].pc_entitytype">;
        const auto s_ID2 = ResId<"[modules:/zitemrepositorykeyentity.class].pc_entitytype">;

        TResourcePtr<ZTemplateEntityFactory> s_Resource, s_Resource2;

        Globals::ResourceManager->GetResourcePtr(s_Resource, s_ID, 0);
        Globals::ResourceManager->GetResourcePtr(s_Resource2, s_ID2, 0);

        Logger::Debug("Resource: {} {}", s_Resource.m_nResourceIndex.val, fmt::ptr(s_Resource.GetResource()));

        if (!s_Resource) {
            Logger::Debug("Resource is not loaded.");
            return;
        }

        ZEntityRef s_NewEntity, s_NewEntity2;
        SExternalReferences s_ExternalRefs;

        Functions::ZEntityManager_NewEntity->Call(
            Globals::EntityManager,
            s_NewEntity,
            "",
            s_Resource,
            s_Scene.m_entityRef,
            s_ExternalRefs,
            -1
        );

        Functions::ZEntityManager_NewEntity->Call(
            Globals::EntityManager,
            s_NewEntity2,
            "",
            s_Resource2,
            s_Scene.m_entityRef,
            s_ExternalRefs,
            -1
        );

        if (!s_NewEntity) {
            Logger::Debug("Failed to spawn entity.");
            return;
        }

        if (!s_NewEntity2) {
            Logger::Debug("Failed to spawn entity2.");
            return;
        }

        const auto s_HitmanSpatialEntity = localHitman.m_entityRef.QueryInterface<ZSpatialEntity>();
        const auto s_ItemSpawner = s_NewEntity.QueryInterface<ZItemSpawner>();

        s_ItemSpawner->m_ePhysicsMode = ZItemSpawner::EPhysicsMode::EPM_DYNAMIC;
        s_ItemSpawner->m_rMainItemKey.m_entityRef = s_NewEntity2;
        s_ItemSpawner->m_rMainItemKey.m_pInterfaceRef = s_NewEntity2.QueryInterface<ZItemRepositoryKeyEntity>();
        s_ItemSpawner->m_rMainItemKey.m_pInterfaceRef->m_RepositoryId = p_RepositoryId;
        s_ItemSpawner->m_bUsePlacementAttach = false;
        auto hitmanWorldMatrix = s_HitmanSpatialEntity->GetObjectToWorldMatrix();
		hitmanWorldMatrix.Pos.z += 2.0f; // Raise the item spawner 1 unit above the Hitman
        s_ItemSpawner->SetObjectToWorldMatrixFromEditor(hitmanWorldMatrix);

        Functions::ZItemSpawner_RequestContentLoad->Call(s_ItemSpawner);
    }
}

void ArchipelagoHitmanCompanion::OnEngineInitialized() {
    Logger::Info("ArchipelagoHitmanCompanion has been initialized!");

    // Register a function to be called on every game frame while the game is in play mode.
    const ZMemberDelegate<ArchipelagoHitmanCompanion, void(const SGameUpdateEvent&)> s_Delegate(this, &ArchipelagoHitmanCompanion::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    // Install a hook to print the name of the scene every time the game loads a new one.
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &ArchipelagoHitmanCompanion::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &ArchipelagoHitmanCompanion::OnClearScene);
}

ArchipelagoHitmanCompanion::~ArchipelagoHitmanCompanion() {
    // Unregister our frame update function when the mod unloads.
    const ZMemberDelegate<ArchipelagoHitmanCompanion, void(const SGameUpdateEvent&)> s_Delegate(this, &ArchipelagoHitmanCompanion::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
}

void ArchipelagoHitmanCompanion::OnDrawMenu() {
    // Toggle our message when the user presses our button.
    if (ImGui::Button(ICON_MD_LOCAL_FIRE_DEPARTMENT " ArchipelagoHitmanCompanion")) {
        m_ShowMessage = !m_ShowMessage;
    }
}

void ArchipelagoHitmanCompanion::OnDrawUI(bool p_HasFocus) {
    if (m_ShowMessage) {
        // Show a window for our mod.
        if (ImGui::Begin("ArchipelagoHitmanCompanion", &m_ShowMessage)) {
			ImGui::InputText("Server Address", m_APServerAddressInput, sizeof(m_APServerAddressInput));
			ImGui::InputText("Slot Name", m_APSlotNameInput, sizeof(m_APSlotNameInput));
            ImGui::InputText("Password", m_APPasswordInput, sizeof(m_APPasswordInput));
            if(!g_APConnected){
                if (ImGui::Button("Connect")) {
                    ConnectToArchipelago();
                }
            }
			/*if (ImGui::Button(ICON_MD_HEART_BROKEN " Suicide")) {
				KillHitman();
            }*/
        }
        ImGui::End();
    }
}

void ArchipelagoHitmanCompanion::OnFrameUpdate(const SGameUpdateEvent &p_UpdateEvent) {    
	// If the AP client exists, poll it for updates. This will trigger our bounce handler if we receive a DeathLink from another player.
    if (g_APClient) {
        g_APClient->poll();
    }

    auto localHitman = SDK()->GetLocalPlayer();
    if(localHitman.m_pInterfaceRef){
        const auto HM5Health = localHitman.m_pInterfaceRef->m_pHealth;
        const float currentHP = HM5Health->m_fHitPoints;
		const float maxHP = HM5Health->m_fMaxHitPoints;
        if (currentHP <= 0.0f && !m_IsHitmanDead && !m_DeathFromDeathLink) {
            Logger::Debug("Hitman Died! with hp of {} out of {}", currentHP, maxHP);
            if (g_APClient && g_APConnected) {
                auto now = std::chrono::system_clock::now();
                double nowDouble = (double)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() / 1000;
                auto alias = g_APClient->get_player_alias(g_APClient->get_player_number());
                auto data = nlohmann::json{
                    {"time", nowDouble},
                    {"cause", alias + " got shot a lot."},
                    {"source", alias + " in HITMAN:WOA" },
                };

                g_APClient->Bounce(data, {}, {}, { "DeathLink" });
                Logger::Debug("Sending Deathlink...");
            }
			m_IsHitmanDead = true;
        }
    }
}

DEFINE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, bool, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters& p_Parameters) {
    Logger::Debug("Loading scene: {}", p_Parameters.m_SceneResource);
    if (m_IsHitmanDead) { m_IsHitmanDead = false; }
	m_DeathFromDeathLink = false;
    return { HookAction::Continue() };
}

DEFINE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, void, OnClearScene, ZEntitySceneContext* th, bool) {
    Logger::Debug("Clearing scene.");

    if (m_IsHitmanDead) { m_IsHitmanDead = false; }
    m_DeathFromDeathLink = false;
    return HookResult<void>(HookAction::Continue());
}

DECLARE_ZHM_PLUGIN(ArchipelagoHitmanCompanion);
