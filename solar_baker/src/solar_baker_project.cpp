#define IMGUI_DEFINE_MATH_OPERATORS

#include "solar_baker_project.h"
#include "imgui.h"
#include <string>
#include <filesystem>
#include <imgui_internal.h>
#include <pugixml.hpp>
#include "app.h"
#include "screen_main.h"


namespace fs = std::filesystem;

static bool gIsCreating = false;
static fs::path gCurrentPath = "";
static char gNewProjectName[200] = {};
static SBProjSource* gCurrentSourceFolder = nullptr;
static SBProjSource* gCurrentSource = nullptr;

SBProject* SBProject::gCurrent = nullptr;

SBProject::SBProject()
{
	rootSource = new SBProjSource();
	rootSource->name = "sources";
}

SBProject::~SBProject()
{

}

void SBProject::Create()
{
	gIsCreating = true;
	gCurrentPath = "C:\\Users\\holyd\\Documents";

	strcpy_s(gNewProjectName, "NoName Mercury Project");
}

void SBProject::Open(const char* path)
{
	gCurrent = new SBProject();
	gCurrent->Load(path);
}

void SBProject::Save()
{
	fs::path xdoc_fname = gCurrentPath / "mproject.xml";

	pugi::xml_document doc;
	
	auto project = doc.append_child("Project");
	project.append_attribute("Name").set_value(gNewProjectName);

	doc.save_file(xdoc_fname.u8string().c_str(), "", 1, pugi::encoding_utf8);
}

void SBProject::SaveAs(const char* path)
{

}

void SBProject::Export(const char* path)
{

}

void SBProject::CreatePAK()
{

}

// Function to draw a folder tree using ImGui
void DrawFolderTree(const std::string& folderPath) {


	// Iterate through the directory entries
	for (const auto& entry : fs::directory_iterator(folderPath, fs::directory_options::skip_permission_denied)) {
		const auto& path = entry.path();

		// Check if the entry is a directory
		if (fs::is_directory(path)) {
			// Create a tree node for the directory
			if (ImGui::TreeNode(path.filename().u8string().c_str())) {
				// Recursively draw the subdirectory tree
				DrawFolderTree(path.u8string());

				// Pop the tree node
				ImGui::TreePop();
			}
		}
	}
}

// Example usage in an ImGui application
void ShowFolderTreeWindow(const std::string& rootFolderPath) {

	fs::path curPath = gCurrentPath;

	if (!curPath.has_parent_path())
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button(".."))
	{
		curPath = curPath.parent_path();
		gCurrentPath = curPath.u8string();
	}

	if (!curPath.has_parent_path())
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	DrawFolderTree(rootFolderPath);
}

void SBProject::Draw()
{
	if (gIsCreating)
	{
		ImGui::Begin("CreateNewProject");

		ImGui::BeginChild("Folders", ImVec2(250, 0), true);
		ShowFolderTreeWindow(gCurrentPath.u8string().c_str());
		ImGui::EndChild();
		ImGui::SameLine();

		auto viewport = ImGui::GetWindowViewport();
		ImGui::BeginChild("Content", ImVec2(viewport->Size.x - 250, 0), true);

		ImGui::InputText("Project Name: ", gNewProjectName, 200);

		fs::path curPath = gCurrentPath;
		curPath /= gNewProjectName;

		ImGui::LabelText("Path:", curPath.u8string().c_str());

		if (ImGui::Button("Create"))
		{
			if (gCurrent != nullptr)
			{
				gCurrent->UnLoad();
			}

			SBProject* newProj = SBProject::CreateNew(gNewProjectName, curPath.u8string().c_str());

			gCurrent = newProj;

			SetMainWindowTitle(gNewProjectName);

			gIsCreating = false;
		}

		ImGui::EndChild();

		ImGui::End();
	}

	if (gCurrent)
	{
		gCurrent->DrawProjectSources();
	}
	
}

SBProject* SBProject::CreateNew(const char* name, const char* path)
{
	SBProject* proj = new SBProject();

	fs::path curPath = path;

	if (fs::exists(curPath))
	{
		fs::remove_all(curPath);
	}


	fs::create_directory(curPath);

	fs::create_directory(curPath / "source");

	fs::create_directory(curPath / "source" / "assets");
	fs::create_directory(curPath / "source" / "shaders");
	fs::create_directory(curPath / "source" / "geometry");
	fs::create_directory(curPath / "source" / "scenes");
	fs::create_directory(curPath / "source" / "textures");
	fs::create_directory(curPath / "source" / "fonts");	

	fs::create_directory(curPath / "intermediate");
	fs::create_directory(curPath / "paks");

	fs::path xdoc_fname = curPath / "mproject.xml";

	pugi::xml_document doc;


	doc.save_file(xdoc_fname.u8string().c_str(), "", 1, pugi::encoding_utf8);

	return proj;
}

void SBProject::Load(const char* pathIn)
{
	this->path = pathIn;

	fs::path xdoc_fname = path / "mproject.xml";

	pugi::xml_document doc;
	doc.load_file(xdoc_fname.u8string().c_str(), 116U, pugi::encoding_utf8);

	auto project = doc.child("Project");

	name = project.attribute("Name").as_string();

	gCurrentPath = path;

	SetMainWindowTitle(name.c_str());

	ReloadSources();
}

void SBProject::UnLoad()
{

}

void LoadSource(fs::path path, SBProjSource* parent)
{
	auto stem = path.stem();

	SBProjSource* current = new SBProjSource();
	current->name = stem.u8string();
	current->SetParent(parent);
	current->fullPath = path;

	for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied))
	{
		if (entry.is_regular_file())
		{
			SBProjShaderSource* source = new SBProjShaderSource();
			source->name = entry.path().filename().u8string();
			source->is_source_file = true;
			source->SetParent(current);
			source->fullPath = entry.path();
			source->CalculateStage();

		}

		if (entry.is_directory())
		{
			LoadSource(entry.path(), current);
		}
	}

	int a = 32;
}

void SBProject::ReloadSources()
{
	fs::path sources_path = path / "source";

	LoadSource(sources_path / "shaders", rootSource);
	LoadSource(sources_path / "geometry", rootSource);
	LoadSource(sources_path / "textures", rootSource);
	LoadSource(sources_path / "fonts", rootSource);
	LoadSource(sources_path / "assets", rootSource);
	LoadSource(sources_path / "scenes", rootSource);
	LoadSource(sources_path / "sounds", rootSource);
}

void DrawSources(SBProjSource* src)
{
	if (!src->is_source_file)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (gCurrentSourceFolder == src)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool is_node_open = ImGui::TreeNodeEx(src->name.c_str(), flags);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			if (gCurrentSourceFolder != src)
			{
				gCurrentSource = nullptr;
			}

			gCurrentSourceFolder = src;			
		}
			

		if (is_node_open)
		{			
			for (auto c : src->childs)
			{
				DrawSources(c);
			}

			ImGui::TreePop();
		}
	}
}

void SBProject::DrawProjectSources()
{
	ImGui::Begin("Sources");

	ImGui::BeginChild("Folders", ImVec2(250, 0), true);
	
	for (auto c : rootSource->childs)
	{
		DrawSources(c);
	}

	ImGui::EndChild();
	ImGui::SameLine();

	auto viewport = ImGui::GetMainViewport();
	ImVec2 srcViewport = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("Content", ImVec2(srcViewport.x - 2, 0), true);

	auto viewport2 = ImGui::GetMainViewport();

	if (gCurrentSourceFolder)
	{
		int texSize = 64;

		int cols = (int)((viewport2->WorkSize.x - 16) / (texSize + 12));

		int index = 0;
		ImVec2 srcRegion = ImGui::GetContentRegionAvail();

		for (auto c : gCurrentSourceFolder->childs)
		{
			ImVec2 curpos = ImGui::GetCursorPos() + ImGui::GetWindowPos() + ImVec2(-ImGui::GetScrollX(), -ImGui::GetScrollY());
			ImVec2 mousePos = ImGui::GetMousePos();
			bool hovered = false;
			bool selected = gCurrentSource == c;

			if (mousePos.x > curpos.x && mousePos.x < curpos.x + texSize
				&& mousePos.y > curpos.y && mousePos.y < curpos.y + texSize)
			{
				hovered = true;
			}

			ImVec4 tint = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
			ImVec4 border = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

			if (selected)
			{
				tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

				border = ImVec4(0.9f, 0.88f, 0.77f, 1.0f);
			}
			else
			{
				if (hovered)
				{
					tint = ImVec4(0.95f, 0.925f, 0.66f, 1.0f);

					border = ImVec4(1.0f, 0.8f, 0.33f, 1.0f);
				}
			}

			//ImGui::Image(img->GetPreview(), ImVec2((float)texSize, (float)texSize), ImVec2(0, 0), ImVec2(1, 1), tint, border);

			auto elSize = ImGui::CalcTextSize(c->name.c_str());
			float curX = ImGui::GetCursorScreenPos().x;
			if (curX + elSize.x + 2 < srcRegion.x)
			{
				ImGui::SameLine();
			}

			ImGui::PushStyleColor(ImGuiCol_Text, selected ? ImVec4(1, 0, 0, 1) : tint);
			ImGui::TextEx(c->name.c_str());
			ImGui::PopStyleColor();

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				if (gCurrentSource != c)
				{
					SBProjShaderSource* shaderSrc = static_cast<SBProjShaderSource*>(c);

					MainScreen::GetMainScreenInstance()->GetShadersScreen()->SetShaderSource(shaderSrc);
					MainScreen::GetMainScreenInstance()->ShowScreenIfNeeded(MainScreen::GetMainScreenInstance()->GetShadersScreen());

					int a = 42;
				}

				gCurrentSource = c;
			}
		}
	}

	ImGui::EndChild();

	ImGui::End();

}