#include <fxpch.h>
#include "AutoComplete.h"
#include "Frostnux/Events/KeyEvent.h"
#include "../uiTools/TextRenderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cctype>
#include <algorithm>

namespace Frostnux {

	const std::vector<std::string> AutoComplete::s_Keywords =
	{
		"alignas","alignof","and","and_eq", "auto","bitand","bitor","bool",
		"char","char8_t","char16_t", "char32_t", "class","compl","concept",
		"const", "consteval","constexpr","constinit", "const_cast", "decltype",
		"delete", "double","dynamic_cast","enum", "explicit","export","extern",
		"false","float", "friend","goto","import","inline", "int", "long",
		"module","mutable","namespace", "new", "noexcept","not"," not_eq ",
		"nullptr", "operator", " or "," or_eq ","private","protected", "public",
		"register","reinterpret_cast","requires", "short", "signed","sizeof",
		"static", "static_assert","static_cast","struct","template", "this",
		"thread_local","throw","true", "try","typedef","typeid","typename",
		"union", "unsigned","using","virtual","void","volatile", "wchar_t",
		" xor "," xor_eq "
	};
	const std::vector<std::string> AutoComplete::s_ControlKeywords =
	{
		"if", "else", "switch", "case", "default", "for", "while", "do", "break", "continue", "co_await", "co_yield", "co_return", "return", "catch"
	};

	AutoComplete::AutoComplete() {}

	AutoComplete::~AutoComplete() {}

	void AutoComplete::Trigger(const TextBuffer& buffer, const Cursor& cursor)
	{
		if (!ShouldTrigger(buffer, cursor))
		{
			Cancel();
			return;
		}
		GenerateCandidates(buffer, cursor);
		if (m_Candidates.empty())
		{
			Cancel();
			return;
		}
		m_IsActive = true;
		m_SelectedIndex = 0;
	}

	void AutoComplete::Cancel()
	{
		m_IsActive = false;
		m_Candidates.clear();
		m_SelectedIndex = 0;
	}

	bool AutoComplete::OnKeyEvent(KeyPressedEvent& e)
	{
		if (!m_IsActive) return false;
		int key = e.GetKeyCode();
		int mods = e.GetMods();

		if (key == FX_KEY_UP)
		{
			m_SelectedIndex = (m_SelectedIndex - 1 + (int)m_Candidates.size()) % m_Candidates.size();
			return true;
		}
		else if (key == FX_KEY_DOWN)
		{
			m_SelectedIndex = (m_SelectedIndex + 1) % m_Candidates.size();
			return true;
		}
		else if (key == FX_KEY_ENTER)
		{
			return true;
		}
		else if (key == FX_KEY_ESCAPE || key == FX_KEY_SPACE)
		{
			Cancel();
			return true;
		}
		else if (key == FX_KEY_BACKSPACE)
		{
			return false;
		}
		return false;
	}

	void AutoComplete::Draw(float parentX, float parentY, float scrollX, float scrollY, float lineHeight)
	{
		if (!m_IsActive || m_Candidates.empty()) return;

		float popupX = m_PopupX;
		float popupY = m_PopupY;
		float itemHeight = lineHeight;
		float maxItems = m_MaxItems;
		float popupWidth = m_PopupWidth;
		float popupHeight = std::min((float)m_Candidates.size(), maxItems) * itemHeight;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.5f, 0.5f, 0.5f, 0.7f);
		glBegin(GL_QUADS);
		glVertex2f(popupX, popupY);
		glVertex2f(popupX + popupWidth, popupY);
		glVertex2f(popupX + popupWidth, popupY + popupHeight);
		glVertex2f(popupX, popupY + popupHeight);
		glEnd();

		glColor4f(0.4f, 0.4f, 0.45f, 1.0f);
		glLineWidth(1.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(popupX, popupY);
		glVertex2f(popupX + popupWidth, popupY);
		glVertex2f(popupX + popupWidth, popupY + popupHeight);
		glVertex2f(popupX, popupY + popupHeight);
		glEnd();

		for (size_t i = 0; i < m_Candidates.size() && i < maxItems; ++i)
		{
			float itemY = popupY + i * itemHeight;
			if (i == m_SelectedIndex)
			{
				glColor4f(0.3f, 0.5f, 0.8f, 0.6f);
				glBegin(GL_QUADS);
				glVertex2f(popupX, itemY);
				glVertex2f(popupX + popupWidth, itemY);
				glVertex2f(popupX + popupWidth, itemY + itemHeight);
				glVertex2f(popupX, itemY + itemHeight);
				glEnd();
			}
			float textX = popupX + 5;
			float textY = itemY + (itemHeight - 18) / 2;
			TextRenderer::Get().DrawText(m_Candidates[i], textX, textY, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		glDisable(GL_BLEND);
	}

	std::string AutoComplete::GetSelectedText() const
	{
		if (m_IsActive && m_SelectedIndex >= 0 && m_SelectedIndex < (int)m_Candidates.size())
		{
			return m_Candidates[m_SelectedIndex];
		}
		return "";
	}

	std::string AutoComplete::GetWordPrefix(const TextBuffer& buffer, const Cursor& cursor) const
	{
		int line = cursor.GetPosition().line;
		int col = cursor.GetPosition().col;
		if (line < 0 || line >= buffer.GetLineCount()) return "";
		const std::string& lineStr = buffer.GetLine(line);
		int start = col;
		while (start > 0 && (std::isalnum(lineStr[start - 1]) || lineStr[start - 1] == '_'))
		{
			start--;
		}
		if (start == col) return "";
		return lineStr.substr(start, col - start);
	}

	bool AutoComplete::ShouldTrigger(const TextBuffer& buffer, const Cursor& cursor) const
	{
		std::string prefix = GetWordPrefix(buffer, cursor);

		return prefix.size() >= 2;
	}

	void AutoComplete::GenerateCandidates(const TextBuffer& buffer, const Cursor& cursor)
	{
		m_Candidates.clear();
		std::string prefix = GetWordPrefix(buffer, cursor);
		if (prefix.empty()) return;

		for (const auto& kw : s_Keywords)
		{
			if (kw.find(prefix) == 0)
			{
				m_Candidates.push_back(kw);
			}
		}
		for (const auto& ckw : s_ControlKeywords)
		{
			if (ckw.find(prefix) == 0)
			{
				m_Candidates.push_back(ckw);
			}
		}

		std::vector<std::string> words;
		for (int i = 0; i < buffer.GetLineCount(); ++i)
		{
			std::string line = buffer.GetLine(i);
			size_t pos = 0;
			while (pos < line.size())
			{
				if (std::isalpha(line[pos]) || line[pos] == '_')
				{
					size_t start = pos;
					while (pos < line.size() && (std::isalnum(line[pos]) || line[pos] == '_')) pos++;
					std::string word = line.substr(start, pos - start);
					if (std::find(words.begin(), words.end(), word) == words.end())
					{
						words.push_back(word);
					}
				}
				else
				{
					pos++;
				}
			}
		}

		for (const auto& word : words)
		{
			if (word.find(prefix) == 0 && std::find(m_Candidates.begin(), m_Candidates.end(), word) == m_Candidates.end()) {
				m_Candidates.push_back(word);
			}
		}

		std::sort(m_Candidates.begin(), m_Candidates.end(), [](const std::string& a, const std::string& b)
			{
				return a.size() < b.size() || (a.size() == b.size() && a < b);
			});
	}

}