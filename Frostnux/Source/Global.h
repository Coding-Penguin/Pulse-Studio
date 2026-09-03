#pragma once

namespace Frostnux {

	static std::string s_RootPath = "H:/Programming/Projects/CppProject/HighPrecisionComputing";
	static std::string s_Version = "2026.9.1"; // "yyyy.a.b.c" => yyyy 年 a 月的第 b 个更新.
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
