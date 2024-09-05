#include "solar_baker_config.h"

#ifdef SB_SHADERS_FRONTEND_HLSL
#include "shader_compiler.h"
#include <set>
#include <algorithm>
#include <fstream>
#include <imgui.h>

#include <d3d12.h>
#include <dxcapi.h>
#include <d3d12shader.h>

#include <locale>
#include <codecvt>
#include <string>
#include <charconv>

IDxcUtils* gUtils = nullptr;
IDxcCompiler3* gCompiler = nullptr;
IDxcIncludeHandler* gIncludeHandler = nullptr;

void ShaderCompiler::initialize()
{
	
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&gUtils));

	IDxcCompiler* compiler = nullptr;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	compiler->QueryInterface(&gCompiler);

	
}

void ShaderCompiler::shutdown()
{

}

std::wstring FromString(std::string str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

void ParseErrors(std::vector<ShaderCompiler::ShaderCompilerErrorInfo>& errors, std::string_view str)
{
	int currentIndex = 0;

	while (true)
	{
		auto nextIndex = str.find("hlsl.hlsl:", currentIndex);

		if (nextIndex >= str.size())
		{
			break;
		}

		auto nextBeforeLine = nextIndex + 10;
		auto nextAfterLine = str.find(':', nextBeforeLine+1)+1;
		auto nextAfterColumn = str.find(':', nextAfterLine+1);
		auto nextBeforeError = str.find(':', nextAfterColumn+1)+2;
		auto nextAfterError = str.find('\n', nextBeforeError);

		auto strLine = str.substr(nextBeforeLine, nextAfterLine - nextBeforeLine - 1);
		auto strColumn = str.substr(nextAfterLine, nextAfterColumn - nextAfterLine);
		auto strError = str.substr(nextBeforeError, nextAfterError - nextBeforeError);

		ShaderCompiler::ShaderCompilerErrorInfo info = {};
		info.error = strError;
		std::from_chars(strColumn.data(), strColumn.data() + strColumn.size(), info.col);
		std::from_chars(strLine.data(), strLine.data() + strLine.size(), info.line);
		--info.col;
		--info.line;
		errors.emplace_back(info);
		currentIndex = nextAfterError + 1;

	}
	
	int a = 42;
}

bool ShaderCompiler::CompileShader(SBProjShaderSource* src, std::vector<ShaderCompilerErrorInfo>& errors)
{
	static const wchar_t* dxcTargets[] = {
	L"vs_6_2"
	,L"ds_6_6"
	,L"hs_6_6"
	,L"gs_6_6"
	,L"ps_6_2"
	,L"lib_6_6"//ray gen
	,L"lib_6_6"//any hit
	,L"lib_6_6"//closest hit
	,L"lib_6_6"//miss
	,L"lib_6_6"//intersect
	,L"lib_6_6"//callable
	,L"as_6_6"
	,L"ms_6_6"
	,L"lib_6_6"//subpass
	,L"lib_6_6" //cluster culling
	,L"lib_6_9" };

	using namespace mercury;

	bool isDebug = false;

	IDxcBlobEncoding* pSource = nullptr;

	auto res = gUtils->CreateBlob(src->cachedSource.c_str(), src->cachedSource.size(), CP_UTF8, &pSource);

	SIZE_T size = pSource->GetBufferSize();
	void* data = pSource->GetBufferPointer();

	res = gUtils->CreateDefaultIncludeHandler(&gIncludeHandler);

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = pSource->GetBufferPointer();
	sourceBuffer.Size = pSource->GetBufferSize();
	BOOL isKnownEncoding = false;
	pSource->GetEncoding(&isKnownEncoding, &sourceBuffer.Encoding);

	std::vector<LPCWSTR> arguments;
	// -E for the entry point (eg. 'main')
	arguments.push_back(L"-E");
	auto entryPointW = FromString(src->entryPoint);

	arguments.push_back(entryPointW.c_str());

	// -T for the target profile (eg. 'ps_6_6')
	arguments.push_back(L"-T");
	arguments.push_back(dxcTargets[(int)src->stage]);



	// Strip reflection data and pdbs (see later)
	arguments.push_back(L"-Fo");
	arguments.push_back(L"-Qstrip_debug");
	arguments.push_back(L"-Qstrip_reflect");
	//arguments.push_back(L"-Qstrip_rootsignature");
	arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
	//arguments.push_back(DXC_ARG_DEBUG); //-Zi
	//arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);

	IDxcResult* compileResult = nullptr;

	res = gCompiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), gIncludeHandler, IID_PPV_ARGS(&compileResult));

	IDxcBlobUtf8* errorsBlob = nullptr;
	IDxcBlobWide* outName = nullptr;

	IDxcBlob* dxilBlob = nullptr;
	IDxcBlobWide* dxilName = nullptr;

	compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorsBlob),&outName);


	//int nameLen = outName->GetStringLength();
	//auto nameStr = outName->GetStringPointer();
	errors.clear();

	int errLen = errorsBlob->GetStringLength();
	auto errStr = errorsBlob->GetStringPointer();
	if (errLen > 0)
	{
		ParseErrors(errors, std::string_view(errStr, errLen));
	}

	compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxilBlob), &dxilName);
	auto bsize = dxilBlob->GetBufferSize();

	int a = 42;

	int textSize = bsize * 6;
	std::string output = "";
	output.reserve(textSize);
	const u8* spirvdata = (const u8*)dxilBlob->GetBufferPointer();
	int curlinesize = 0;

	for (int i = 0; i < bsize; ++i)
	{
		u8 b = spirvdata[i];

		constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',	   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

		if (i != 0)
			output += ',';

		output += "0x";
		output += hexmap[(b & 0xF0) >> 4];
		output += hexmap[b & 0x0F];

		curlinesize++;
		if (curlinesize > 20)
		{
			output += '\n';
			curlinesize = 0;
		}

	}

	ImGui::SetClipboardText(output.c_str());

	return true;
}
#endif //SB_SHADERS_FRONTEND_GLSL