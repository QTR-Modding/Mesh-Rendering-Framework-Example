#include "Hooks.h"
#include "Logger.h"
#include "MeshRenderingFrameworkAPI.h"
#include "UI.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {

        int i = 0;
        const auto getNifPath = [](RE::TESBoundObject* object) -> const char* {
            if (!object) {
                return nullptr;
            }

            if (auto weapon = object->As<RE::TESObjectWEAP>()) {
                if (auto firstPersonModel = weapon->firstPersonModelObject) {
                    return firstPersonModel->GetModel();
                }
            }
            if (auto model = object->As<RE::TESModel>()) {
                return model->GetModel();
            }
            if (auto spell = object->As<RE::SpellItem>()) {
                if (auto displayObject = spell->GetMenuDisplayObject()) {
                    if (auto model = displayObject->As<RE::TESModel>()) {
                        return model->GetModel();
                    }
                }
            }

            return nullptr;
        };

        const auto renderAndLogNifPath = [&i, &getNifPath](RE::TESBoundObject* object) {
            if (!object) {
                logger::info("Could not render mesh because the form was not found");
                return;
            }

            const auto filePath = std::format(".\\Data\\{:03}.png", i++);
            MeshRenderingFrameworkAPI::Mesh::Render(filePath.c_str(), object, 1024, 1024);

            const auto nifPath = getNifPath(object);
            if (nifPath && nifPath[0]) {
                logger::info("Rendered NIF '{}' to '{}'; file exists: {}", nifPath, filePath, std::filesystem::exists(filePath));
            } else {
                logger::info("Rendered mesh with no NIF path to '{}'; file exists: {}", filePath, std::filesystem::exists(filePath));
            }
        };

        constexpr RE::FormID TREE = 0x1306D;
        renderAndLogNifPath(RE::TESForm::LookupByID<RE::TESBoundObject>(TREE));

        const auto& [map, lock] = RE::TESForm::GetAllForms();
        const RE::BSReadWriteLock l{lock};
        for (auto& [id, form] : *map) {
            if (form) {
                if (auto weap = form->As<RE::TESObjectWEAP>()) {
                    renderAndLogNifPath(weap);
                    RE::PlayerCharacter::GetSingleton()->AddObjectToContainer(weap, nullptr, 1, nullptr);
                }
                if (auto spell = form->As<RE::SpellItem>()) {
                    renderAndLogNifPath(spell);
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
