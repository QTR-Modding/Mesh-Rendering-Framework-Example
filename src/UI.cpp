#include "UI.h"
#include "MeshRenderingFrameworkAPI.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Main", Main::Render);
    SKSEMenuFramework::AddSectionItem("Debug", Debug::Render);
}

class MenuItem {
    MeshRenderingFrameworkAPI::OrbitMesh* mesh = nullptr;
public:
    ~MenuItem() {
        if (mesh) {
            delete mesh;
        }
    }
    MenuItem(MeshRenderingFrameworkAPI::OrbitMesh* mesh, bool alwaysUpdate = false) : mesh(mesh) {
        if (mesh) {
            mesh->SetAlwaysUpdate(alwaysUpdate);
        }
    }
    bool PlayAnimation(
        const char* gameAnimationPath,
        bool loop = true,
        const char* skeletonPath = "meshes\\actors\\character\\character assets\\skeleton.hkx") {
        return mesh && mesh->PlayAnimation(gameAnimationPath, loop, skeletonPath);
    }
    bool SetExpression(RE::BSFaceGenKeyframeMultiple::Expression expression) {
        return mesh && mesh->SetExpression(expression);
    }
    void SetFraming(float scale, RE::NiPoint3 position) {
        if (!mesh) {
            return;
        }
        mesh->ScaleUp(scale);
        mesh->SetPosition(position);
    }
    void ResetLighting() {
        if (!mesh) {
            return;
        }

        mesh->ClearLights();
        mesh->AddLight(-0.45f, 0.55f, 0.70f, 1.05f, 0.98f, 0.88f, 1.0f);
        mesh->AddLight(0.65f, 0.35f, 0.25f, 0.42f, 0.50f, 0.62f, 1.0f);
        mesh->AddLight(0.10f, -0.65f, 0.55f, 0.30f, 0.35f, 0.42f, 1.0f);
        mesh->SetExposure(1.0f);
    }
    void RenderLightingControls() {
        if (!mesh) {
            return;
        }

        ImGuiMCP::Text("Lighting");

        float exposure = 1.0f;
        if (mesh->GetExposure(exposure) &&
            ImGuiMCP::SliderFloat("Exposure##DwarvenSpiderLighting", &exposure, 0.0f, 4.0f)) {
            mesh->SetExposure(exposure);
        }

        const std::uint32_t lightCount = mesh->GetLightCount();
        ImGuiMCP::Text("Directional lights: %u / 8", lightCount);

        if (ImGuiMCP::Button("Add light##DwarvenSpiderLighting")) {
            mesh->AddLight(-0.4f, 0.6f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button("Clear lights##DwarvenSpiderLighting")) {
            mesh->ClearLights();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button("Reset lighting##DwarvenSpiderLighting")) {
            ResetLighting();
        }

        for (std::uint32_t lightIndex = 0; lightIndex < lightCount; ++lightIndex) {
            MeshRenderingFrameworkAPI::Internal::ILight* light = mesh->GetLight(lightIndex);
            if (!light) {
                continue;
            }

            ImGuiMCP::Text("Light %u", lightIndex + 1);

            float direction[3]{};
            if (MeshRenderingFrameworkAPI::Internal::ILight_GetDirection(
                    light, &direction[0], &direction[1], &direction[2])) {
                const std::string label = std::format("Direction##DwarvenSpiderLight{}", lightIndex);
                if (ImGuiMCP::DragFloat3(label.c_str(), direction, 0.01f, -1.0f, 1.0f)) {
                    MeshRenderingFrameworkAPI::Internal::ILight_SetDirection(
                        light, direction[0], direction[1], direction[2]);
                }
            }

            float color[3]{};
            if (MeshRenderingFrameworkAPI::Internal::ILight_GetColor(
                    light, &color[0], &color[1], &color[2])) {
                const std::string label = std::format("Color##DwarvenSpiderLight{}", lightIndex);
                if (ImGuiMCP::ColorEdit3(label.c_str(), color)) {
                    MeshRenderingFrameworkAPI::Internal::ILight_SetColor(
                        light, color[0], color[1], color[2]);
                }
            }

            float strength = 1.0f;
            if (MeshRenderingFrameworkAPI::Internal::ILight_GetStrength(light, &strength)) {
                const std::string label = std::format("Strength##DwarvenSpiderLight{}", lightIndex);
                if (ImGuiMCP::SliderFloat(label.c_str(), &strength, 0.0f, 5.0f)) {
                    MeshRenderingFrameworkAPI::Internal::ILight_SetStrength(light, strength);
                }
            }
        }
    }
    void Render(const char* name) {
        if (ImGuiMCP::Button((std::string("Save##Save") + name).c_str())) {
            const auto path = std::format(".\\Data\\{}.png", name);
            mesh->Save(path.c_str());
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button((std::string("Reset##Reset") + name).c_str())) {
            RE::NiMatrix3 orientation;
            orientation.SetEulerAnglesXYZ(0.0f, 0.0f, 0.0f);
            mesh->SetOrbitOrientation(orientation);
        }
        mesh->Render((std::string("##MeshOrbitControl") + name).c_str());
    }
};

#define GOLD_INGOT 0x12E49
#define TREE 0x1306D
#define DWE_SPIDER 0x10EC86
#define FLAMES_SPELL 0x12FCD
#define FROSTBITE_SPELL 0x2B96B
#define HEALING_SPELL 0x12FCC
#define LYDIA 0xA2C8E

void __stdcall UI::Main::Render() {
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

    static MenuItem* lydiaHead = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaHeadAnimationLoaded = lydiaHead->PlayAnimation(
        "meshes\\actors\\character\\animations\\dialogueangryhips.hkx",
        true);

    static MenuItem* lydiaWholeNpcAnimated =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaAnimationLoaded = lydiaWholeNpcAnimated->PlayAnimation(
        "meshes\\actors\\character\\animations\\mt_idle_a_arms_crossedloop.hkx",
        true);
    static bool lydiaExpressionApplied = true;

    static MenuItem* goldIngot = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(GOLD_INGOT, 1024, 1024));
    static MenuItem* tree = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(TREE, 1024, 1024));
    static MenuItem* dwarvenSpider = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(DWE_SPIDER, 1024, 1024));
    static const bool dwarvenSpiderAnimationLoaded = dwarvenSpider->PlayAnimation(
        "meshes\\actors\\dwarvenspider\\animations\\mainidle.hkx",
        true,
        "meshes\\actors\\dwarvenspider\\character assets\\skeleton.hkx");
    ImGuiMCP::Text(
        lydiaHeadAnimationLoaded
            ? "Lydia - skeletal head motion with direct TRI expressions"
            : "Lydia - failed to animate head");
    lydiaHead->Render("3");

    ImGuiMCP::Text(
        lydiaAnimationLoaded
            ? "Lydia - skeletal animation with direct TRI expressions"
            : "Lydia - failed to load animation");
    ImGuiMCP::Text("HKX animates the skeleton. Expression TRI test:");
    if (ImGuiMCP::Button("Happy##LydiaExpression")) {
        const bool headApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueHappy);
        const bool wholeNpcApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueHappy);
        lydiaExpressionApplied = headApplied && wholeNpcApplied;
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Angry##LydiaExpression")) {
        const bool headApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueAnger);
        const bool wholeNpcApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::DialogueAnger);
        lydiaExpressionApplied = headApplied && wholeNpcApplied;
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("Neutral##LydiaExpression")) {
        const bool headApplied = lydiaHead->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral);
        const bool wholeNpcApplied = lydiaWholeNpcAnimated->SetExpression(
            RE::BSFaceGenKeyframeMultiple::Expression::MoodNeutral);
        lydiaExpressionApplied = headApplied && wholeNpcApplied;
    }
    if (!lydiaExpressionApplied) {
        ImGuiMCP::Text("Failed to load or match Lydia's expression TRI morph.");
    }
    lydiaWholeNpcAnimated->Render("9");

    goldIngot->Render("4");
    tree->Render("5");
    ImGuiMCP::Text(
        dwarvenSpiderAnimationLoaded
            ? "Dwarven Spider - game animation: mainidle.hkx"
            : "Dwarven Spider - failed to load game animation");
    dwarvenSpider->RenderLightingControls();
    dwarvenSpider->Render("6");
}

void __stdcall UI::Debug::Render() {
    static MenuItem* lydiaWholeNpc =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
        static MenuItem* lydiaWholeNpcWithoutArmour =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024, false));
    ImGuiMCP::Text("Lydia - whole NPC without armour");
    lydiaWholeNpcWithoutArmour->Render("8");
    ImGuiMCP::Text("Lydia - whole NPC");
    lydiaWholeNpc->Render("7");


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
