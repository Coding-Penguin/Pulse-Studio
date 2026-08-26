#include <Frostnux.h>

class ExampleLayer : public Frostnux::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate(float deltaTime) override
	{
	}

	bool OnEvent(Frostnux::Event& event) override
	{
		return false;
	}
};

class SandboxApp : public Frostnux::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Frostnux::uiLayer());
	}
	~SandboxApp()
	{
	}
};

Frostnux::Application* Frostnux::CreateApplication()
{
	return new SandboxApp();
}

int main()
{
	Frostnux::Application* app = Frostnux::CreateApplication();

	app->Run();

	delete app;
	return 0;
}
