#include "Renderer3DSettingsPanel.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/Renderer3D.h"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>


namespace Engine {
	extern const std::filesystem::path g_AssetPath;

	void Renderer3DSettingsPanel::OnImGuiRender()
	{

		ImGui::Begin("Renderer3D Settings");
		DrawElements();
		ImGui::End();
	}
	static const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	template< typename UIFunction>
	static void DrawElement(const std::string& name, UIFunction uiFunction) {

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y + 2.0f;
		ImGui::Separator();
		ImGui::PopStyleVar();
		bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags, name.c_str());

		if (open) {
			uiFunction();
			ImGui::TreePop();
		}
	}

	void Renderer3DSettingsPanel::DrawElements() {
		DrawElement("Sky Box", []() {
			static bool isChecked ;
			ImGui::Checkbox("Sky Box", &isChecked);
			if (isChecked) {
				Renderer3D::SetSkyBoxRenderer(true);
				for (size_t i = 0; i < 6; i++) {
					std::string title = "Texture" + std::to_string(i);
					ImGui::Button(title.c_str(), ImVec2(100.0f, 0.0f));
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							Renderer3D::AddCubeTextureFace( texturePath.string(), i);
						}
						ImGui::EndDragDropTarget();
					}
				}
			}
			else {
				Renderer3D::SetSkyBoxRenderer(false);
			}
		});
	}
}
