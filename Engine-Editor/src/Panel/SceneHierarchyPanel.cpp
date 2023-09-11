#include "SceneHierarchyPanel.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Engine/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](auto entityID) {

			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectionContext = {};
		}

		//right click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Crete Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();
		ImGui::Begin("Properties");
		if (m_SelectionContext) {
			DrawComponent(m_SelectionContext);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("Add Component");

			if (ImGui::BeginPopup("Add Component")) {
				if (ImGui::MenuItem("Camera")) {
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Sprite Renderer")) {
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		//right click on entity
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {

			ImGui::TreePop();
		}
		if (entityDeleted) {
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
			m_Context->DestroyEntity(entity);
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth =100.0f) {
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f,0.1f,0.15f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f,0.2f,0.2f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f,0.1f,0.15f,1.0f });
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f,0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f,0.7f,0.3f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f,0.8f,0.3f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f,0.7f,0.3f,1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PopStyleColor(3);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f,0.35f,0.9f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		//ImGui::SameLine();
		ImGui::PopStyleColor(3);


		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();

	}

	void SceneHierarchyPanel::DrawComponent(Entity entity) {
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			//Camera0

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (entity.HasComponent<TransformComponent>()) {

			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");

			if (open) {
				auto& transform = entity.GetComponent<TransformComponent>();
				DrawVec3Control("Position", transform.Translation);
				glm::vec3 rotation = glm::degrees(transform.Rotation);
				DrawVec3Control("Rotation", rotation);
				transform.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", transform.Scale,1.0f);
				ImGui::TreePop();
			}

		}

		if (entity.HasComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera")) {

				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
					for (int i = 0; i < 2; i++) {

						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFov());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveFOV))
						camera.SetPerspectiveVerticalFov(glm::radians(perspectiveFOV));

					float orthoNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetPerspectiveNearClip(orthoNear);

					float orthoFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetPerspectiveFarClip(orthoFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);

				}

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");

			ImGui::SameLine(ImGui::GetWindowWidth() -25.0f);
			if (ImGui::Button("+", ImVec2{ 20.0f,20.0f })) {
				ImGui::OpenPopup("Component Settings");
			}
			ImGui::PopStyleVar();
			bool removeComponent = false;
			if (ImGui::BeginPopup("Component Settings")) {
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open) {
				
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));

				ImGui::TreePop();
			}
			if (removeComponent)
				entity.RemoveComponent<SpriteRendererComponent>();
		}
	}
}