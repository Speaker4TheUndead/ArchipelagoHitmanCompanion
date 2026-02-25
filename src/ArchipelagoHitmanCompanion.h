#pragma once

#include <IPluginInterface.h>

#include <Glacier/ZScene.h>

struct LogMessage {
    std::string text;
};

class MessageLog {
public:
    void AddMessage(const std::string& msg);
    const std::vector<LogMessage>& GetMessages() const;
private:
    std::vector<LogMessage> messages;
};

class ArchipelagoHitmanCompanion : public IPluginInterface {
public:
    void OnEngineInitialized() override;
    ~ArchipelagoHitmanCompanion() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;
    void ConnectToArchipelago();
    void KillHitman();
    void DrawLogWindow();

private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
    DECLARE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, bool, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters& p_Parameters);
	DECLARE_PLUGIN_DETOUR(ArchipelagoHitmanCompanion, void, OnClearScene, ZEntitySceneContext* th, bool bFullyUnloadScene);

private:
    bool m_ShowMessage = false;
	bool m_IsHitmanDead = false;
	bool m_DeathFromDeathLink = false;
    bool m_ShowLogWindow = false;
	char m_APServerAddressInput[2000] = "localhost:38281";
	char m_APSlotNameInput[2000] = "Player";
	char m_APPasswordInput[2000] = "";
    const char m_GameName[30] = "HITMAN World of Assassination";
    MessageLog m_MessageLog;
};


DEFINE_ZHM_PLUGIN(ArchipelagoHitmanCompanion)
