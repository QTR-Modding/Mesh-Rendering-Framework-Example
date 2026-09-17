#include "UI.h"
#include "MeshRenderingFrameworkAPI.h"
#include "MenuItem.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Meshes/Player Character", PlayerCharacter::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Lydia Head", LydiaHead::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Lydia Animated", LydiaAnimated::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Gold Ingot", GoldIngot::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Tree", Tree::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Dwarven Spider", DwarvenSpider::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Lydia Whole NPC", LydiaWholeNpc::Render);
    SKSEMenuFramework::AddSectionItem("Meshes/Lydia Without Armour", LydiaWithoutArmour::Render);
    SKSEMenuFramework::AddSectionItem("Debug", Debug::Render);
}



#define GOLD_INGOT 0x12E49
#define TREE 0x1306D
#define DWE_SPIDER 0x10EC86
#define FLAMES_SPELL 0x12FCD
#define FROSTBITE_SPELL 0x2B96B
#define HEALING_SPELL 0x12FCC
#define LYDIA 0xA2C8E

void __stdcall UI::PlayerCharacter::Render() {
    static MenuItem* playerCharacter = nullptr;
    static RE::NiAVObject* capturedPlayer3D = nullptr;
    static bool playerAnimationLoaded = false;

    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    RE::NiAVObject* currentPlayer3D = player ? player->Get3D(false) : nullptr;
    const bool refreshPlayer = ImGuiMCP::Button("Refresh player character");
    if (currentPlayer3D &&
        (refreshPlayer || !playerCharacter || currentPlayer3D != capturedPlayer3D)) {
        delete playerCharacter;
        playerCharacter = new MenuItem(
            new MeshRenderingFrameworkAPI::OrbitMesh(player, 1024, 1024),
            true);
        playerCharacter->SetFraming(3.0f, RE::NiPoint3{0.0f, -500.0f, -180.0f});
        playerAnimationLoaded = playerCharacter->PlayAnimation(
            "meshes\\actors\\character\\animations\\mt_idle_a_arms_crossedloop.hkx",
            true);
        capturedPlayer3D = currentPlayer3D;
    }

    ImGuiMCP::Text(
        playerAnimationLoaded
            ? "Player character - animated idle with current appearance and equipped armor"
            : "Player character - current appearance and equipped armor (animation unavailable)");
    if (playerCharacter) {
        playerCharacter->Render("PlayerCharacter");
    } else {
        ImGuiMCP::Text("Player character 3D is not loaded yet.");
    }
}

void __stdcall UI::LydiaHead::Render() {
    static MenuItem* lydiaHead = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaHeadAnimationLoaded = lydiaHead->PlayAnimation(
        "meshes\\actors\\character\\animations\\dialogueangryhips.hkx",
        true);
    static bool expressionApplied = true;

    ImGuiMCP::Text(
        lydiaHeadAnimationLoaded
            ? "Lydia - skeletal head motion with direct TRI expressions"
            : "Lydia - failed to animate head");
    ImGuiMCP::Text("Expression TRI test:");
    if (ImGuiMCP::Button("Happy##LydiaHeadExpression")) {
        expressionApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueHappy);
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Angry##LydiaHeadExpression")) {
        expressionApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueAnger);
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Neutral##LydiaHeadExpression")) {
        expressionApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral);
    }
    if (!expressionApplied) {
        ImGuiMCP::Text("Failed to load or match Lydia's expression TRI morph.");
    }
    lydiaHead->Render("3");
}

void __stdcall UI::LydiaAnimated::Render() {
    static MenuItem* lydiaWholeNpcAnimated =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaAnimationLoaded = lydiaWholeNpcAnimated->PlayAnimation(
        "meshes\\actors\\character\\animations\\mt_idle_a_arms_crossedloop.hkx",
        true);
    static bool expressionApplied = true;

    ImGuiMCP::Text(
        lydiaAnimationLoaded
            ? "Lydia - skeletal animation with direct TRI expressions"
            : "Lydia - failed to load animation");
    ImGuiMCP::Text("HKX animates the skeleton. Expression TRI test:");
    if (ImGuiMCP::Button("Happy##LydiaAnimatedExpression")) {
        expressionApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueHappy);
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Angry##LydiaAnimatedExpression")) {
        expressionApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueAnger);
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Neutral##LydiaAnimatedExpression")) {
        expressionApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral);
    }
    if (!expressionApplied) {
        ImGuiMCP::Text("Failed to load or match Lydia's expression TRI morph.");
    }
    lydiaWholeNpcAnimated->Render("9");
}

void __stdcall UI::GoldIngot::Render() {
    static MenuItem* goldIngot = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(GOLD_INGOT, 1024, 1024));
    goldIngot->Render("4");
}

void __stdcall UI::Tree::Render() {
    static MenuItem* tree = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(TREE, 1024, 1024));
    tree->Render("5");
}

void __stdcall UI::DwarvenSpider::Render() {
    static MenuItem* dwarvenSpider = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(DWE_SPIDER, 1024, 1024));
    static const bool dwarvenSpiderAnimationLoaded = dwarvenSpider->PlayAnimation(
        "meshes\\actors\\dwarvenspider\\animations\\mainidle.hkx",
        true,
        "meshes\\actors\\dwarvenspider\\character assets\\skeleton.hkx");

    ImGuiMCP::Text(
        dwarvenSpiderAnimationLoaded
            ? "Dwarven Spider - game animation: mainidle.hkx"
            : "Dwarven Spider - failed to load game animation");
    dwarvenSpider->RenderLightingControls();
    dwarvenSpider->Render("6");
}

void __stdcall UI::LydiaWholeNpc::Render() {
    static MenuItem* lydiaWholeNpc =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    ImGuiMCP::Text("Lydia - whole NPC");
    lydiaWholeNpc->Render("7");
}

void __stdcall UI::LydiaWithoutArmour::Render() {
    static MenuItem* lydiaWholeNpcWithoutArmour =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024, false));
    ImGuiMCP::Text("Lydia - whole NPC without armour");
    lydiaWholeNpcWithoutArmour->Render("8");
}

void __stdcall UI::Debug::Render() {
    if (ImGuiMCP::Button("Generate Images")) {
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
}
