#pragma once
#include <vector>
#include <string>
#include "Frostnux/Events/Event.h"
#include "Frostnux/Events/KeyEvent.h"
#include "TextBuffer.h"
#include "Cursor.h"

namespace Frostnux {

	class AutoComplete
	{
	public:
		AutoComplete();
		~AutoComplete();

		void Trigger(const TextBuffer& buffer, const Cursor& cursor);
		void Cancel();
		bool IsActive() const { return m_IsActive; }

		bool OnKeyEvent(KeyPressedEvent& e);
		void Draw(float parentX, float parentY, float scrollX, float scrollY, float lineHeight);

		std::string GetSelectedText() const;
		void SetPopupPosition(float x, float y) { m_PopupX = x; m_PopupY = y; }
		std::string GetWordPrefix(const TextBuffer& buffer, const Cursor& cursor) const;

	private:
		bool m_IsActive = false;
		std::vector<std::string> m_Candidates;
		int m_SelectedIndex = 0;
		float m_PopupX, m_PopupY;
		float m_PopupWidth = 200.0f;
		float m_MaxItems = 10;

		void GenerateCandidates(const TextBuffer& buffer, const Cursor& cursor);
		bool ShouldTrigger(const TextBuffer& buffer, const Cursor& cursor) const;

		static const std::vector<std::string> s_Keywords;
		static const std::vector<std::string> s_ControlKeywords;
	};

}