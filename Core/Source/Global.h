#pragma once

namespace CodePen {

	static std::string s_RootPath = "H:/Programming/Projects/CppProject/HighPrecisionComputing";
	static std::string s_Version = "2026.1.13"; // "yyyy.a.b[c.d.f]" => yyyy年第 a 个大版本更新中的第 b 个小版本更新; 内部版本号为 [c.d], 现在为[0.1.23], 但仅在 Hub 中显示, 其中 c为内部大版本号 (即 yyyy 减 2026), , d 为内部小版本号 (即 b), f 为 c 中的累计版本号.
	static float s_FontSize = 20.0f;
	static bool s_FontLoaded = false;

	static bool CreateFolder(const std::string& path)
	{
		try
		{
			if (std::filesystem::create_directories(path))
			{
				PS_CORE_INFO("Created directory: {}", path);
				return true;
			}
			else
			{
				PS_CORE_WARN("Directory already exists: {}", path);
				return true;
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			PS_CORE_ERROR("Failed to create directory: {}", e.what());
			return false;
		}
	}

}
