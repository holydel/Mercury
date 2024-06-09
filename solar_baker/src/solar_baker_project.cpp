#include "solar_baker_project.h"
#include "imgui.h"
#include <string>
#include <filesystem>
#include <imgui_internal.h>
#include <pugixml.hpp>

namespace fs = std::filesystem;

static bool gIsCreating = false;
static std::string gCurrentPath = "";
static char gNewProjectName[200] = {};

SBProject* SBProject::gCurrent = nullptr;

void SBProject::Create()
{
	gIsCreating = true;
    gCurrentPath = "C:\\Users\\holyd\\Documents";

    strcpy_s(gNewProjectName, "NoName Mercury Project");
}

void SBProject::Open(const char* path)
{

}

void SBProject::Save()
{

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
    for (const auto& entry : fs::directory_iterator(folderPath,fs::directory_options::skip_permission_denied)) {
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
        ShowFolderTreeWindow(gCurrentPath);
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

            gIsCreating = false;
        }

        ImGui::EndChild();

		ImGui::End();
	}
}

SBProject* SBProject::CreateNew(const char* name, const char* path)
{
    SBProject* proj = new SBProject();

    fs::path curPath = path;

    if (!fs::exists(curPath))
    {
        fs::create_directory(curPath);

        fs::path xdoc_fname = curPath / "mproject.xml";

        pugi::xml_document doc;


        doc.save_file(xdoc_fname.u8string().c_str(), "", 1, pugi::encoding_utf8);
    }
    return proj;
}

void SBProject::Load()
{

}

void SBProject::UnLoad()
{

}