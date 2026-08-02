#include "UI.h"
#include "MeshRenderingFrameworkAPI.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Main", Main::Render);
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



void __stdcall UI::Main::Render() {
    #define GOLD_INGOT 0x12E49
    #define TREE 0x1306D
    #define DWE_SPIDER 0x10EC86
    #define FLAMES_SPELL 0x12FCD
    #define FROSTBITE_SPELL 0x2B96B
    #define HEALING_SPELL 0x12FCC
    #define LYDIA 0xA2C8E
    static MenuItem* lydiaHead = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(LYDIA, 1024, 1024));
    static MenuItem* lydiaWholeNpc =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    static MenuItem* lydiaWholeNpcAnimated =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024));
    static const bool lydiaAnimationLoaded = lydiaWholeNpcAnimated->PlayAnimation(
        "meshes\\actors\\character\\animations\\mt_idle_a_arms_crossedloop.hkx",
        true);
    static MenuItem* lydiaWholeNpcWithoutArmour =
        new MenuItem(new MeshRenderingFrameworkAPI::WholeNpcOrbitMesh(LYDIA, 1024, 1024, false));
    static MenuItem* goldIngot = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(GOLD_INGOT, 1024, 1024));
    static MenuItem* tree = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(TREE, 1024, 1024));
    static MenuItem* dwarvenSpider = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(DWE_SPIDER, 1024, 1024));
    static const bool dwarvenSpiderAnimationLoaded = dwarvenSpider->PlayAnimation(
        "meshes\\actors\\dwarvenspider\\animations\\mainidle.hkx",
        true,
        "meshes\\actors\\dwarvenspider\\character assets\\skeleton.hkx");
    ImGuiMCP::Text("Lydia - head");
    lydiaHead->Render("3");
    ImGuiMCP::Text("Lydia - whole NPC");
    lydiaWholeNpc->Render("7");
    ImGuiMCP::Text(
        lydiaAnimationLoaded
            ? "Lydia - game animation: mt_idle_a_arms_crossedloop.hkx"
            : "Lydia - failed to load game animation");
    lydiaWholeNpcAnimated->Render("9");
    ImGuiMCP::Text("Lydia - whole NPC without armour");
    lydiaWholeNpcWithoutArmour->Render("8");
    goldIngot->Render("4");
    tree->Render("5");
    ImGuiMCP::Text(
        dwarvenSpiderAnimationLoaded
            ? "Dwarven Spider - game animation: mainidle.hkx"
            : "Dwarven Spider - failed to load game animation");
    dwarvenSpider->Render("6");
}
