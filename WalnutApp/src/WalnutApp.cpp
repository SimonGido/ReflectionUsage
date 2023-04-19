#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"

#include "Reflection.h"

#include "../entt/include/entt/entt.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace XYZ;

static void DrawUI(const char* name, std::string& value)
{
	char nameBuffer[_MAX_PATH];
	memcpy(nameBuffer, value.c_str(), value.size());
	nameBuffer[value.size()] = 0;

	if (ImGui::InputText(name, nameBuffer, _MAX_PATH))
	{
		value = nameBuffer;
	}
}

static void DrawUI(const char* name, glm::vec3& value)
{
	ImGui::DragFloat3(name, glm::value_ptr(value));
}
static void DrawUI(const char* name, glm::vec2& value)
{
	ImGui::DragFloat2(name, glm::value_ptr(value));
}
static void DrawUI(const char* name, float& value)
{
	ImGui::DragFloat(name, &value);
}
static void DrawUI(const char* name, bool& value)
{
	ImGui::Checkbox(name, &value);
}


template <typename T>
static void DrawComponent(const char* name, T& component)
{
	ImGui::Text(name);
	Utils::For<Reflection<T>::sc_NumVariables>([&](auto i) {
		auto variableName = Reflection<T>::GetName<i.value>();
		auto& variable = Reflection<T>::Get<i.value>(component);
		DrawUI(variableName.data(), variable);
	});
}

struct NameComponent
{
	std::string Name;
};
REFLECTABLE(NameComponent, Name)

struct TransformComponent
{
	glm::vec3 Translation, Rotation, Scale;
	TransformComponent()
		: Translation(0), Rotation(0), Scale(1)
	{}
};
REFLECTABLE(TransformComponent, Translation, Rotation, Scale)

struct BoxColliderComponent
{
	glm::vec3 Size;
	glm::vec3 Offset;

	BoxColliderComponent()
		: Size(0), Offset(0)
	{}
};
REFLECTABLE(BoxColliderComponent, Size, Offset)



REGISTER_REFLECTABLES(COMPONENTS, NameComponent, TransformComponent, BoxColliderComponent)

class ExampleLayer : public Walnut::Layer
{
public:
	entt::registry m_Registry;

	std::vector<entt::entity> m_Entities;

	ExampleLayer()
	{
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<NameComponent>(entity);
		m_Registry.emplace<TransformComponent>(entity);
		m_Registry.emplace<BoxColliderComponent>(entity);
		m_Entities.push_back(entity);

		entity = m_Registry.create();
		m_Registry.emplace<NameComponent>(entity);
		m_Registry.emplace<TransformComponent>(entity);
		m_Registry.emplace<BoxColliderComponent>(entity);
		m_Entities.push_back(entity);
	}

	virtual void OnUIRender() override
	{
		if (ImGui::Begin("Components"))
		{
			for (auto entity : m_Entities)
			{
				ImGui::Text(std::to_string((std::uint32_t)entity).c_str());

				Utils::For<COMPONENTS::sc_NumClasses>([&](auto i) {

					auto reflClass = COMPONENTS::Get<i.value>();
					if (HasComponent(entity, reflClass))
					{
						auto className = COMPONENTS::GetName<i.value>();
						auto& component = GetComponent(entity, reflClass);

						const char* name = className.data();
						DrawComponent(name, component);
					}
				});
			}
		}
		ImGui::End();
	}

	template <typename T>
	T& GetComponent(entt::entity entity, Reflection<T> refl)
	{
		return m_Registry.get<T>(entity);
	}

	template <typename T>
	bool HasComponent(entt::entity entity, Reflection<T> refl)
	{
		return m_Registry.any_of<T>(entity);
	}

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}