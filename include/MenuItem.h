#pragma once

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
    bool PlayAnimation(const char* gameAnimationPath, bool loop = true, const char* skeletonPath = "meshes\\actors\\character\\character assets\\skeleton.hkx") { return mesh && mesh->PlayAnimation(gameAnimationPath, loop, skeletonPath); }
    bool SetExpression(RE::BSFaceGenKeyframeMultiple::Expression expression) { return mesh && mesh->SetExpression(expression); }
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
        if (mesh->GetExposure(exposure) && ImGuiMCP::SliderFloat("Exposure##DwarvenSpiderLighting", &exposure, 0.0f, 4.0f)) {
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
            if (MeshRenderingFrameworkAPI::Internal::ILight_GetDirection(light, &direction[0], &direction[1], &direction[2])) {
                const std::string label = std::format("Direction##DwarvenSpiderLight{}", lightIndex);
                if (ImGuiMCP::DragFloat3(label.c_str(), direction, 0.01f, -1.0f, 1.0f)) {
                    MeshRenderingFrameworkAPI::Internal::ILight_SetDirection(light, direction[0], direction[1], direction[2]);
                }
            }

            float color[3]{};
            if (MeshRenderingFrameworkAPI::Internal::ILight_GetColor(light, &color[0], &color[1], &color[2])) {
                const std::string label = std::format("Color##DwarvenSpiderLight{}", lightIndex);
                if (ImGuiMCP::ColorEdit3(label.c_str(), color)) {
                    MeshRenderingFrameworkAPI::Internal::ILight_SetColor(light, color[0], color[1], color[2]);
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
        if (ImGuiMCP::Button((std::string("Save Render Image##SaveRenderImage") + name).c_str())) {
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