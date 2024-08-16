#include "application.h"
#include "platform.h"
#include "engine.h"
#include <filesystem>
#include "pugixml.hpp"
#include <unordered_map>

using namespace mercury;

Application* gApplication = nullptr;

//settings cache
static std::unordered_map<std::string_view, std::string> gSettingsValues;
static std::unordered_map<std::string_view, std::vector<std::string>> gSettingsLists;

Application::Application()
{
	gApplication = this;
}

Application::~Application()
{
	gApplication = nullptr;
}

bool Application::Initialize()
{
	return true;
}

bool Application::Shutdown()
{
	return true;
}

bool Application::Update()
{
	return true;
}

void Application::OnApplicationClose()
{

}

/// <summary>
/// run before client gApplication->Initialize();
/// </summary>
/// 

namespace fs = std::filesystem;
fs::path gAppPath;
fs::path gAppName;
fs::path gConfigName;

fs::path gFullnameConfigPath;
fs::path gFullnameConfigFileName;

void ApplicationInitializeSettingsCache()
{
	gAppPath =  platform::getApplicationDataPath();
	gAppName = gApplication->config.appName;
	gConfigName = "config.xml";

	gFullnameConfigPath = gAppPath / gAppName;
	gFullnameConfigFileName = gFullnameConfigPath / gConfigName;

	if (fs::exists(gFullnameConfigFileName))
	{
		pugi::xml_document xdoc;
		xdoc.load_file(gFullnameConfigFileName.c_str(), 116, pugi::encoding_utf8);
	}

}

void ApplicationSaveSettingsCache()
{
	if (!gSettingsValues.empty() && gSettingsLists.empty())
	{
		if (!fs::exists(gFullnameConfigPath))
		{
			fs::create_directories(gFullnameConfigPath);
		}

		pugi::xml_document xdoc;
		xdoc.save_file(gFullnameConfigFileName.c_str(), "", 1, pugi::encoding_utf8);
	}
}

void ApplicationRun()
{
	engine::initialize();

#ifdef MERCURY_DESKTOP
	platform::createMainWindow();
#endif

	ApplicationInitializeSettingsCache();
	gApplication->Initialize();
	while (gApplication->Update())
	{
		engine::update();
	}

	ApplicationSaveSettingsCache();
	gApplication->Shutdown();

#ifdef MERCURY_DESKTOP
	platform::destroyMainWindow();
#endif

	engine::shutdown();
}

const std::string& Application::GetRuntimeSettingsValue(const char* u8key)
{
	auto value = gSettingsValues.find(u8key);

	if (value != gSettingsValues.end())
	{
		return (*value).second;
	}

    static std::string emptyString = {};
	return emptyString;
}

const std::vector<std::string>& Application::GetRuntimeSettingsList(const char* u8key)
{	
	auto value = gSettingsLists.find(u8key);

	if (value != gSettingsLists.end())
	{
		return (*value).second;
	}

    static std::vector<std::string> emptyStringVec = {};
    return emptyStringVec;
}

void Application::SetRuntimeSettingsValue(const char* u8key, const std::string& value)
{
	gSettingsValues[u8key] = value;
}

void Application::SetRuntimeSettingsList(const char* u8key, const std::vector<std::string>& value)
{
	gSettingsLists[u8key] = value;
}

void Application::SetWindowTitle(const char* u8string)
{
	platform::setMainWindowTitle(u8string);
}