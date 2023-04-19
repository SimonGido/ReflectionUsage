#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"

#include "Reflection.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace XYZ;

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
		std::string variableName = std::string(Reflection<T>::sc_VariableNames[i.value]);
		auto& variable = Reflection<T>::Get<i.value>(component);
		DrawUI(variableName.c_str(), variable);
	});
}


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



REGISTER_REFLECTABLES(COMPONENTS, TransformComponent, BoxColliderComponent)

class ExampleLayer : public Walnut::Layer
{
public:
	TransformComponent transform;
	BoxColliderComponent boxCollider;

	virtual void OnUIRender() override
	{
		if (ImGui::Begin("Components"))
		{		
			Utils::For<COMPONENTS::sc_NumClasses>([&](auto i) {

				std::string className = std::string(COMPONENTS::sc_ClassNames[i.value]);
				auto reflClass = COMPONENTS::Get<i.value>();
				auto& component = GetComponent(reflClass);
				DrawComponent(className.c_str(), component);
			});		
		}
		ImGui::End();
	}

	template <typename T>
	T& GetComponent(Reflection<T> refl)
	{
		if constexpr (std::is_same_v<T, TransformComponent>)
		{
			return transform;
		}
		else
		{
			return boxCollider;
		}
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