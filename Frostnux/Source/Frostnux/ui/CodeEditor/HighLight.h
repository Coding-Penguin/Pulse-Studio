#pragma once
#include <fxpch.h>
#include "glm/glm.hpp"

namespace Frostnux {

	enum class HighlightColor
	{
		Default,
		Keyword,
		ControlKeyword,
		String,
		Comment,
		Number,
		Preprocessor,
		Function,
		ClassName,
		Namespace,
		Macro
	};

	struct HighlightSpan
	{
		int start, end;
		HighlightColor color;
	};

	enum class Language
	{
		CPP,
		Python,
		Java,
		CSharp,
		Markdown,
		JSON,
		Lua,
		Rust,
		Text
	};

	class Highlight
	{
	public:
		Highlight(const Language& mode = Language::CPP);
		void SetLanguage(const Language& mode);
		std::vector<HighlightSpan> HighlightLine(const std::string& line) const;

		glm::vec3 GetColorForHighlight(HighlightColor color) const
		{
			switch (color)
			{
			case HighlightColor::Keyword:
			case HighlightColor::ControlKeyword:	return glm::vec3(0.3f, 0.7f, 1.0f);
			case HighlightColor::String:			return glm::vec3(0.8f, 0.6f, 0.2f);
			case HighlightColor::Comment:			return glm::vec3(0.3f, 0.7f, 0.3f);
			case HighlightColor::Number:			return glm::vec3(0.7f, 0.7f, 0.8f);
			case HighlightColor::Preprocessor:		return glm::vec3(0.5f, 0.5f, 0.5f);
			case HighlightColor::Function:			return glm::vec3(0.7f, 0.5f, 0.2f);
			case HighlightColor::ClassName:			return glm::vec3(0.2f, 0.5f, 0.3f);
			case HighlightColor::Namespace:			return glm::vec3(0.5f, 0.5f, 0.3f);
			case HighlightColor::Macro:				return glm::vec3(0.7f, 0.5f, 0.2f);
			default:
				if (ThemeManager::IsDarkTheme())
					return glm::vec3(0.9f, 0.9f, 0.93f);
				else
					return glm::vec3(0.1f, 0.1f, 0.13f);
			}
		}
	private:
		std::vector<std::string> m_Keywords;
		std::vector<std::string> m_ControlKeywords;
		Language m_languageMode;

		mutable bool m_InBlockComment = false;

		void InitCppKeywords();
		std::vector<HighlightSpan> HighlightCppLine(const std::string& line) const;
		std::vector<HighlightSpan> HighlightGenericLine(const std::string& line) const;
	};

}
