#include "pspch.h"
#include "uiLayer.h"
#include "uiTools/ui.h"
#include "CodePen/Log.h"
#include "CodePen/Application.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "CodeEditor/CodeEditor.h"
#include "CodePen/SettingsManager.h"

namespace CodePen {

	uiLayer& uiLayer::Get()
	{
		static uiLayer instance;
		return instance;
	}

	uiLayer::uiLayer() : Layer("UILayer")
	{
	}

	uiLayer::~uiLayer() 
	{
		for (auto* win : m_Windows)
			delete win;
		m_Windows.clear();
		delete m_TitleBar;
		delete m_StatusBar;
		delete m_ShortcutBar;
		delete m_CodeEditor;
	}

	void uiLayer::OnAttach()
	{
		PS_CORE_INFO("uiLayer attached.");

		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();

		auto& settings = SettingsManager::Get().GetSettings();

		m_CodeEditor = new CodeEditor("");

		m_StatusBar = new uiStatusBar();
		m_StatusBar->OnAttach();

		m_TitleBar = new uiTitleBar();
		m_TitleBar->OnAttach();

		m_ShortcutBar = new uiShortcutBar();
		m_ShortcutBar->OnAttach();
		std::vector<ShortcutItem> fileGroup =
		{
			{ "new", "N", "New File", []() { PS_INFO("New File"); } },
			{ "open", "O", "Open File...", []() { PS_INFO("Open File"); } },
			{ "save", "S", "Save File", []() { PS_INFO("Save File"); } },
			{ "saveall", "SA", "Save All Files", []() { PS_INFO("Save All"); } }
		};
		std::vector<ShortcutItem> editGroup =
		{
			{ "undo", "U", "Undo", []() { PS_INFO("Undo"); }},
			{ "redo", "R", "Redo", []() { PS_INFO("Redo"); } },
			{ "cut", "Ct", "Cut", []() { PS_INFO("Cut"); } },
			{ "copy", "Co", "Copy", []() { PS_INFO("Copy"); } },
			{ "paste", "P", "Paste", []() { PS_INFO("Paste"); } }
		};
		std::vector<ShortcutItem> buildGroup =
		{
			{ "debug", "D", "Debug", []() { PS_INFO("Debug"); } },
			{ "build", "B", "Build", []() { PS_INFO("Start Build."); } },
			{ "rebuild", "RB", "Rebuild", []() { PS_INFO("Start Rebuild"); } },
			{ "clean", "Cl", "Clean", []() { PS_INFO("Clean"); } },
			{ "run", "R", "Run", []() { PS_INFO("Run"); } }
		};
		std::vector<ShortcutItem> bookmarkGroup =
		{
			{ "findbookmark", "FB", "Find Bookmark", []() { PS_INFO("Find Bookmark"); } },
			{ "addbookmark", "AB", "Add Bookmark", []() { PS_INFO("Add Bookmark"); } },
			{ "deletebookmark", "DB", "Delete Bookmark", []() { PS_INFO("Delete Bookmark"); } },
			{ "nextbookmark", "NB", "Next Bookmark", []() { PS_INFO("Next Bookmark"); } },
			{ "clearbookmarks", "CB", "Clear Bookmarks", []() { PS_INFO("Clear Bookmarks"); } }
		};
		m_ShortcutBar->AddGroup(fileGroup, true);
		m_ShortcutBar->AddGroup(editGroup, true);
		m_ShortcutBar->AddGroup(buildGroup, true);
		m_ShortcutBar->AddGroup(bookmarkGroup, false);

		uiWindow::InitDockSystem(0.0f, 110.0f, width, height - 150.0f);

		auto* properties = new PropertiesWindow();
		auto* fileExplorer = new FileExplorer(s_RootPath, properties);
		fileExplorer->SetFileOpenCallback([this](const std::string& path)
			{
				if (this->m_TabManager)
				{
					this->m_TabManager->OpenFile(path);
				}
				else
				{
					PS_CORE_ERROR("TabManager is null!");
				}
			});
		auto* output = new uiWindow("Output");
		auto* notifications = new uiWindow("Notifications");
		m_Windows.push_back(fileExplorer);
		m_Windows.push_back(output);
		uiWindow::DockWindow(fileExplorer, DockRegion::Left);
		uiWindow::DockWindow(output, DockRegion::Bottom);
		for (auto* win : m_Windows)
		{
			win->OnAttach();
		}

		m_TabManager = new EditorTabManager();
		int topOffset = 110;
		int bottomOffset = 40;
		float tabX = 0;
		float tabY = topOffset;
		float tabW = width;
		float tabH = height - topOffset - bottomOffset;
		m_TabManager->SetBounds(tabX, tabY, tabW, tabH);
		for (const auto& filepath : settings.openFiles)
		{
			m_TabManager->OpenFile(filepath, true);
		}
	}

	void uiLayer::OnDetach() 
	{
		for (auto* win : m_Windows)
			win->OnDetach();
		if (m_TitleBar) m_TitleBar->OnDetach();
		if (m_StatusBar) m_StatusBar->OnDetach();
		if (m_ShortcutBar) m_ShortcutBar->OnDetach();
		delete m_CodeEditor;
	}

	void uiLayer::OnUpdate(float deltaTime)
	{
		auto& app = Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		double mx, my;
		glfwGetCursorPos(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &mx, &my);

		float centerX = uiWindow::GetCenterX();
		float centerY = uiWindow::GetCenterY();
		float centerW = uiWindow::GetCenterW();
		float centerH = uiWindow::GetCenterH();

		float leftW = uiWindow::GetDynamicLeftWidth();
		float rightW = uiWindow::GetDynamicRightWidth();
		float bottomH = uiWindow::GetDynamicBottomHeight();

		MouseCircle::Get().OnUpdate(deltaTime);

		uiWindow::DrawDockAreas();

		if (m_StatusBar)
		{
			m_StatusBar->OnUpdate(deltaTime);
			m_StatusBar->SetStatusText("Ready");
		}

		DockRegion preview = uiWindow::GetPreviewRegion();
		if (preview != DockRegion::None)
		{
			float x = 0, y = 0, w = 0, h = 0;
			float mainW = uiWindow::GetMainW();
			float mainH = uiWindow::GetMainH();
			float centerW = mainW - leftW - rightW;
			float centerH = mainH - bottomH;

			switch (preview)
			{
			case DockRegion::Left:
				x = 0; y = 0; w = leftW; h = centerH;
				break;
			case DockRegion::Right:
				x = mainW - rightW; y = 0; w = rightW; h = centerH;
				break;
			case DockRegion::Bottom:
				x = 0; y = mainH - bottomH; w = mainW; h = bottomH;
				break;
			case DockRegion::Center:
				x = leftW; y = 0; w = centerW; h = centerH;
				break;
			default: break;
			}

			x += uiWindow::GetMainX();
			y += uiWindow::GetMainY();

			glEnable(GL_BLEND);
			glColor4f(0.2f, 0.5f, 0.8f, 0.5f);
			glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
			glEnd();
		}

		if (m_TitleBar) m_TitleBar->OnUpdate(deltaTime);

		if (m_ShortcutBar)
		{
			m_ShortcutBar->OnUpdate(deltaTime);
			m_ShortcutBar->Draw();
		}

		if (m_CodeEditor)
		{
			m_CodeEditor->SetViewBounds(centerX, centerY + 30, centerW, centerH - 30);
			m_CodeEditor->OnUpdate(deltaTime);
		}

		if (m_TabManager)
		{
			m_TabManager->SetBounds(centerX, centerY, centerW, centerH);

			m_TabManager->OnUpdate(deltaTime);
			m_TabManager->Draw();
		}

		for (auto* win : m_Windows)
		{
			win->OnUpdate(deltaTime);
		}

		uiWindow::DrawDockPanel(mx, my);
	}

	bool uiLayer::OnEvent(Event& event)
	{
		MouseCircle::Get().OnEvent(event);

		if (m_TabManager && m_TabManager->OnEvent(event))
			return true;

		if (m_TitleBar && m_TitleBar->OnEvent(event))
			return true;

		if (m_ShortcutBar && m_ShortcutBar->OnEvent(event))
			return true;

		for (auto* win : m_Windows)
		{
			if (win->OnEvent(event))
				return true;
		}

		if (m_CodeEditor) return m_CodeEditor->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
			{
			int width = e.GetWidth(), height = e.GetHeight();

			Application& app = Application::Get();
			float leftW = app.GetWindow().GetWidth() * 0.2f,
				rightW = app.GetWindow().GetWidth() * 0.2f,
				bottomH = app.GetWindow().GetHeight() * 0.3f;
			float mainW = (float)width;
			float mainH = (float)height;
			float centerW = mainW - leftW - rightW;
			float centerH = mainH - bottomH;
			float yOffset = 110.0f;

			EditorView::Get().SetBounds(leftW, yOffset, centerW, centerH);

			return false;
			});

		return false;
	}

	void uiLayer::AddWindow(uiWindow* window)
	{
		if (window)
		{
			m_Windows.push_back(window);
			window->OnAttach();
		}
	}

	bool uiLayer::IsPointOverAnyWindow(float x, float y)
	{
		auto& instance = Get();
		for (auto* win : instance.m_Windows)
		{
			if (win->IsVisible())
			{
				float wx = win->GetX();
				float wy = win->GetY();
				float ww = win->GetWidth();
				float wh = win->GetHeight();

				if (x >= wx && x <= wx + ww && y >= wy && y <= wy + wh)
				{
					return true;
				}
			}
		}
		return false;
	}

}
