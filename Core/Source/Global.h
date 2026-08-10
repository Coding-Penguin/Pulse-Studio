#pragma once

namespace CodePen {

	static bool s_FontLoaded = false;
	static float s_FontSize = 20.0f;
	static std::string s_RootPath = "H:/Programming/Projects/CppProject/ProtoCity";

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
