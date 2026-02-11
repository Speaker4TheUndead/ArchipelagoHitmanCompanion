#pragma once

#include <IPluginInterface.h>

#include <Glacier/ZScene.h>

class ArchipelagoHitmanCompanion : public IPluginInterface {
public:
    void OnEngineInitialized() override;
    ~ArchipelagoHitmanCompanion() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;
    void ConnectToArchipelago();
    void KillHitman();

private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
    DECLARE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, bool, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters& p_Parameters);
	DECLARE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, void, OnClearScene, ZEntitySceneContext* th, bool bFullyUnloadScene);

private:
    bool m_ShowMessage = false;
	bool m_IsHitmanDead = false;
	bool m_DeathlinkPending = false;
	char m_APServerAddressInput[2000] = "localhost:";
	char m_APSlotNameInput[2000] = "Henry";
	char m_APPasswordInput[2000] = "";
    const char m_GameName[30] = "HITMAN World of Assassination";
};

DEFINE_ZHM_PLUGIN(ArchipelagoHitmanCompanion)
