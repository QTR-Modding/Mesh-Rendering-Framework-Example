#include "UI.h"
#include "MeshRenderingFrameworkAPI.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Main", Main::Render);
    SKSEMenuFramework::AddSectionItem("Debug", Main::Render);
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

    static MenuItem* lydiaHead = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(LYDIA, 1024, 1024));

    static MenuItem* lydiaWholeNpcAnimated =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaAnimationLoaded = lydiaWholeNpcAnimated->PlayAnimation(
        "meshes\\actors\\character\\animations\\mt_idle_a_arms_crossedloop.hkx",
        true);

    static MenuItem* goldIngot = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(GOLD_INGOT, 1024, 1024));
    static MenuItem* tree = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(TREE, 1024, 1024));
    static MenuItem* dwarvenSpider = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(DWE_SPIDER, 1024, 1024));
    static const bool dwarvenSpiderAnimationLoaded = dwarvenSpider->PlayAnimation(
        "meshes\\actors\\dwarvenspider\\animations\\mainidle.hkx",
        true,
        "meshes\\actors\\dwarvenspider\\character assets\\skeleton.hkx");
    ImGuiMCP::Text("Lydia - head");
    lydiaHead->Render("3");

    ImGuiMCP::Text(
        lydiaAnimationLoaded
            ? "Lydia - game animation: mt_idle_a_arms_crossedloop.hkx"
            : "Lydia - failed to load game animation");
    lydiaWholeNpcAnimated->Render("9");

    goldIngot->Render("4");
    tree->Render("5");
    ImGuiMCP::Text(
        dwarvenSpiderAnimationLoaded
            ? "Dwarven Spider - game animation: mainidle.hkx"
            : "Dwarven Spider - failed to load game animation");
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
