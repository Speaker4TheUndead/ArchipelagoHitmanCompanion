#pragma once

#include <IPluginInterface.h>

#include <Glacier/ZScene.h>

class MyMod : public IPluginInterface {
public:
    void OnEngineInitialized() override;
    ~MyMod() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
    DECLARE_PLUGIN_DETOUR(MyMod, bool, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters& p_Parameters);

private:
    bool m_ShowMessage = false;
};

DEFINE_ZHM_PLUGIN(MyMod)
