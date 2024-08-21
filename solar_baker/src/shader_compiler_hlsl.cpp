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


bool ShaderCompiler::CompileShader(SBProjShaderSource* src)
{
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
	arguments.push_back(L"mainVS");

	// -T for the target profile (eg. 'ps_6_6')
	arguments.push_back(L"-T");
	arguments.push_back(L"ps_6_6");

	// Strip reflection data and pdbs (see later)
	arguments.push_back(L"-Qstrip_debug");
	arguments.push_back(L"-Qstrip_reflect");

	//arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
	arguments.push_back(DXC_ARG_DEBUG); //-Zi

	IDxcResult* compileResult = nullptr;

	res = gCompiler->Compile(&sourceBuffer, arguments.data(), arguments.size(), gIncludeHandler, IID_PPV_ARGS(&compileResult));

	IDxcBlobUtf8* errorsBlob = nullptr;
	IDxcBlobWide* outName = nullptr;

	compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorsBlob),&outName);

	//int nameLen = outName->GetStringLength();
	//auto nameStr = outName->GetStringPointer();

	int errLen = errorsBlob->GetStringLength();
	auto errStr = errorsBlob->GetStringPointer();

	int a = 42;

	std::vector<ShaderCompilerErrorInfo> errors;

	return true;
}
#endif //SB_SHADERS_FRONTEND_GLSL