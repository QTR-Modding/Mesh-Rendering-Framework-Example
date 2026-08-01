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
    static auto item1 = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(GOLD_INGOT, 1024, 1024));
    static auto item2 = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(TREE, 1024, 1024));
    static auto item3 = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(DWE_SPIDER, 1024, 1024));
    static auto item4 = new MenuItem(new MeshRenderingFrameworkAPI::OrbitMesh(FLAMES_SPELL, 1024, 1024), true);
    item1->Render("1");
    item2->Render("2");
    item3->Render("3");
    item4->Render("4");
}
