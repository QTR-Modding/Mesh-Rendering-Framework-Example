#include "Hooks.h"
#include "Logger.h"
#include "MeshRenderingFrameworkAPI.h.h"
#include "UI.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {

        int i = 0;
        #define TREE 0x1306D
        MeshRenderingFrameworkAPI::Mesh::Render(std::format(".\\Data\\{:03}.png", i++).c_str(), TREE, 1024, 1024);

        const auto& [map, lock] = RE::TESForm::GetAllForms();
        const RE::BSReadWriteLock l{lock};
        for (auto& [id, form] : *map) {
            if (form) {
                if (auto weap = form->As<RE::TESObjectWEAP>()) {
                    MeshRenderingFrameworkAPI::Mesh::Render(std::format(".\\Data\\{:03}.png", i++).c_str(), weap, 1024, 1024);
                    RE::PlayerCharacter::GetSingleton()->AddObjectToContainer(weap, nullptr, 1, nullptr);
                }
                if (auto spell = form->As<RE::SpellItem>()) {
                    MeshRenderingFrameworkAPI::Mesh::Render(std::format(".\\Data\\{:03}.png", i++).c_str(), spell, 1024, 1024);
                }
            }
            if (i > 100) {
                break;
            }
        }
    }
    if (message->type == SKSE::MessagingInterface::kPostLoad) {
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();
    Hooks::Install();
    return true;
}