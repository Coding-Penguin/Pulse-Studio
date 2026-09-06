#pragma once
#include "../uiTools/uiWindow.h"
#include "TextBuffer.h"
#include "Cursor.h"
#include "HighLight.h"
#include "EditorView.h"
#include "Frostnux/Events/KeyEvent.h"
#include "AutoComplete.h"
#include <GLFW/glfw3.h>

namespace Frostnux {

	class CodeEditor
	{
	public:
		static CodeEditor& Get();

		CodeEditor();
		CodeEditor(const std::string& path);
		virtual ~CodeEditor();

		virtual void OnUpdate(float deltaTime);
		virtual bool OnEvent(Event& event);

		void LoadFile(const std::string& path);
		void SaveFile(const std::string& path);
		std::string GetText() const;

		void SetSyntaxMode(const CodeLanguage& mode);

		void Copy();
		void Cut();
		void Paste();
		bool HasSelection() const { return m_Cursor.HasSelection(); }
		std::string GetSelectedText() const;
		void DeleteSelection();

		void SetViewBounds(float x, float y, float w, float h) { m_View->SetBounds(x, y, w, h); }
		std::string GetFilePath() const { return m_Buffer.GetFilePath(); }

		std::string GetFileExtension(const std::string& path) const;

		void SetText(const std::string& text);

		static EditorView* GetView();

		void SaveFile();
		void SaveFileAs(const std::string& path);
		static bool IsModified() { return m_IsModified; }
	private:
		static CodeEditor instance;

		TextBuffer m_Buffer;
		Cursor m_Cursor;
		Highlight m_Highlighter;
		static EditorView* m_View;

		GLFWcursor* m_ArrowCursor;
		GLFWcursor* m_IBeamCursor;

		bool m_MouseDragSelecting = false;
		CursorPosition m_MouseDragStart;

		struct UndoAction
		{
			enum Type { Insert, Delete, InsertNewline, DeleteNewline };
			std::string data;
			Type type;

			int line, col;
			int endLine, endCol;
		};
		std::stack<UndoAction> m_UndoStack;
		std::stack<UndoAction> m_RedoStack;

		static bool m_IsModified;

		AutoComplete m_AutoComplete;
		void InsertAutoCompleteText(const std::string& text);

		void RecordAction(const UndoAction& action);
		void Undo();
		void Redo();
		void ClearRedoStack();

		void RecordInsert(int line, int col, char ch);
		void RecordDelete(int line, int col, char ch);
		void RecordInsertNewline(int line, int col);
		void RecordDeleteNewline(int line, int col, const std::string& nextLineContent);

		void ProcessKeyEvent(KeyPressedEvent& e);
		void ProcessCharEvent(CharEvent& e);
		void ProcessMouseButton(MouseButtonPressedEvent& e);

		void ReplaceSelection(const std::string& text);
	};

}