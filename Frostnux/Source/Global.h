#pragma once

namespace Frostnux {

	static std::string s_RootPath = "H:/Programming/Projects/CppProject/HighPrecisionComputing";
	static std::string s_Version = "2026.1.15"; // "yyyy.a.b" => yyyy 年第 a 个大版本更新中的第 b 个小版本更新.
	static float s_FontSize = 20.0f;
	static bool s_FontLoaded = false;

	static bool CreateFolder(const std::string& path)
	{
		try
		{
			if (std::filesystem::create_directories(path))
			{
				FX_CORE_INFO("Created directory: {}", path);
				return true;
			}
			else
			{
				FX_CORE_WARN("Directory already exists: {}", path);
				return true;
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			FX_CORE_ERROR("Failed to create directory: {}", e.what());
			return false;
		}
	}

}
