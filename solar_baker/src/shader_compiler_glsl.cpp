#include "solar_baker_config.h"

#ifdef SB_SHADERS_FRONTEND_GLSL
#include "shader_compiler.h"
#include <set>
#include <algorithm>
#include <fstream>
#include <imgui.h>
#include "glslang/Public/ShaderLang.h"
#include <glslang/Public/ResourceLimits.h>
#include <glslang/MachineIndependent/localintermediate.h>
#include "SPIRV/GlslangToSpv.h"

void ShaderCompiler::initialize()
{
	ShInitialize();
}

void ShaderCompiler::shutdown()
{
	ShFinalize();
}

class DirStackFileIncluder : public glslang::TShader::Includer {
public:
	DirStackFileIncluder() : externalLocalDirectoryCount(0) { }

	virtual IncludeResult* includeLocal(const char* headerName,
		const char* includerName,
		size_t inclusionDepth) override
	{
		return readLocalPath(headerName, includerName, (int)inclusionDepth);
	}

	virtual IncludeResult* includeSystem(const char* headerName,
		const char* /*includerName*/,
		size_t /*inclusionDepth*/) override
	{
		return readSystemPath(headerName);
	}

	// Externally set directories. E.g., from a command-line -I<dir>.
	//  - Most-recently pushed are checked first.
	//  - All these are checked after the parse-time stack of local directories
	//    is checked.
	//  - This only applies to the "local" form of #include.
	//  - Makes its own copy of the path.
	virtual void pushExternalLocalDirectory(const std::string& dir)
	{
		directoryStack.push_back(dir);
		externalLocalDirectoryCount = (int)directoryStack.size();
	}

	virtual void releaseInclude(IncludeResult* result) override
	{
		if (result != nullptr) {
			delete[] static_cast<tUserDataElement*>(result->userData);
			delete result;
		}
	}

	virtual std::set<std::string> getIncludedFiles()
	{
		return includedFiles;
	}

	virtual ~DirStackFileIncluder() override { }

protected:
	typedef char tUserDataElement;
	std::vector<std::string> directoryStack;
	int externalLocalDirectoryCount;
	std::set<std::string> includedFiles;

	// Search for a valid "local" path based on combining the stack of include
	// directories and the nominal name of the header.
	virtual IncludeResult* readLocalPath(const char* headerName, const char* includerName, int depth)
	{
		// Discard popped include directories, and
		// initialize when at parse-time first level.
		directoryStack.resize(depth + externalLocalDirectoryCount);
		if (depth == 1)
			directoryStack.back() = getDirectory(includerName);

		// Find a directory that works, using a reverse search of the include stack.
		for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it) {
			std::string path = *it + '/' + headerName;
			std::replace(path.begin(), path.end(), '\\', '/');
			std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
			if (file) {
				directoryStack.push_back(getDirectory(path));
				includedFiles.insert(path);
				return newIncludeResult(path, file, (int)file.tellg());
			}
		}

		return nullptr;
	}

	// Search for a valid <system> path.
	// Not implemented yet; returning nullptr signals failure to find.
	virtual IncludeResult* readSystemPath(const char* /*headerName*/) const
	{
		return nullptr;
	}

	// Do actual reading of the file, filling in a new include result.
	virtual IncludeResult* newIncludeResult(const std::string& path, std::ifstream& file, int length) const
	{
		char* content = new tUserDataElement[length];
		file.seekg(0, file.beg);
		file.read(content, length);
		return new IncludeResult(path, content, length, content);
	}

	// If no path markers, return current working directory.
	// Otherwise, strip file name and return path leading up to it.
	virtual std::string getDirectory(const std::string path) const
	{
		size_t last = path.find_last_of("/\\");
		return last == std::string::npos ? "." : path.substr(0, last);
	}
};

void ParseErrors(const std::string& infoLog, std::vector<ShaderCompiler::ShaderCompilerErrorInfo>& errors)
{
	size_t offset = 0;
	size_t prevOff = std::string::npos;
	std::vector<std::string> outputErr;

	while (offset < infoLog.size())
	{
		size_t noff = infoLog.find("ERROR: ", offset);

		if (noff != std::string::npos)
		{
			if (prevOff != std::string::npos)
			{
				outputErr.push_back(infoLog.substr(prevOff, noff - prevOff));
			}

			prevOff = noff;
			offset = noff + 7;
		}
		else
		{
			if (prevOff != std::string::npos)
			{
				outputErr.push_back(infoLog.substr(prevOff));
			}

			offset = infoLog.size();
		}
	}

	for (std::string& s : outputErr)
	{
		int cpos[3] = { 0 };
		int ccol = 0;

		for (int i = 0; i < s.length() && ccol < 3; ++i)
		{
			if (s[i] == ':')
			{
				cpos[ccol] = i;
				ccol++;
			}
		}

		if (ccol == 3) //have line and row of error
		{
			ShaderCompiler::ShaderCompilerErrorInfo cei;
			auto srow = s.substr(cpos[0] + 2, cpos[1] - cpos[0] - 2);
			auto scol = s.substr(cpos[1] + 1, cpos[2] - cpos[1] - 1);

			cei.line = std::stoi(srow);
			cei.col = std::stoi(scol);
			cei.error = s.substr(cpos[2] + 2);

			errors.push_back(cei);
		}
		else
		{
			ShaderCompiler::ShaderCompilerErrorInfo cei;

			cei.line = -1;
			cei.col = -1;
			cei.error = s.substr(cpos[0] + 2);

			errors.push_back(cei);
		}
	}
}

EShLanguage MercuryStageToGLSLangStage(mercury::Shader::Stage stage)
{
	using namespace mercury;
	switch (stage)
	{
	case Shader::Stage::Vertex:
		return EShLanguage::EShLangVertex;
	case Shader::Stage::Fragment:
		return EShLanguage::EShLangFragment;
	case Shader::Stage::Compute:
		return EShLanguage::EShLangCompute;
	case Shader::Stage::Geometry:
		return EShLanguage::EShLangGeometry;
	case Shader::Stage::TessControl:
		return EShLanguage::EShLangTessControl;
	case Shader::Stage::TessEvaluation:
		return EShLanguage::EShLangTessEvaluation;
	case Shader::Stage::Mesh:
		return EShLanguage::EShLangMesh;
	case Shader::Stage::Task:
		return EShLanguage::EShLangTask;

	case Shader::Stage::RayGen:
		return EShLanguage::EShLangRayGen;
	case Shader::Stage::AnyHit:
		return EShLanguage::EShLangAnyHit;
	case Shader::Stage::Miss:
		return EShLanguage::EShLangMiss;
	case Shader::Stage::ClosestHit:
		return EShLanguage::EShLangClosestHit;
	case Shader::Stage::Callable:
		return EShLanguage::EShLangCallable;
	default:
		return EShLanguage::EShLangVertex;
	}
}

bool ShaderCompiler::CompileShader(SBProjShaderSource* src)
{
	using namespace mercury;

	bool isDebug = true;

	std::vector<ShaderCompilerErrorInfo> errors;

	glslang::TProgram* program = new glslang::TProgram();


	EShLanguage curStage = MercuryStageToGLSLangStage(src->stage);

	glslang::TShader* shader = new glslang::TShader(curStage);

	std::vector<const char*> source_strings;

	const char* arr[2] = { 0,0 };
	std::string packedDefinesStr = "";

	source_strings.push_back(src->cachedSource.c_str());

	shader->setStrings(source_strings.data(), source_strings.size());
	shader->setEntryPoint("main");

	shader->setNanMinMaxClamp(false);

	if (false)
		shader->setEnhancedMsgs();

	if (false)
		shader->setDebugInfo(true);

	shader->setOverrideVersion(450);

	glslang::EShClient Client = glslang::EShClientVulkan;
	glslang::EShTargetClientVersion ClientVersion = glslang::EShTargetVulkan_1_0;

	glslang::EShTargetLanguage TargetLanguage = glslang::EShTargetSpv;
	glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

	int Options = 0;
	int ClientInputSemanticsVersion = 100;



	shader->setEnvInput(glslang::EShSourceGlsl,
		curStage, Client, ClientInputSemanticsVersion);

	shader->setEnvClient(Client, ClientVersion);
	shader->setEnvTarget(TargetLanguage, TargetVersion);

	DirStackFileIncluder includer;
	std::string str;
	EShMessages messages = EShMsgAST;

	bool resultPreprop = shader->preprocess(GetDefaultResources(), 450, ECoreProfile, false, false, messages, &str, includer);


	if (!resultPreprop)
	{
		std::string infoLog = shader->getInfoLog();
		std::string debugLog = shader->getInfoDebugLog();

		ParseErrors(shader->getInfoLog(), errors);

		return false;
	}

	bool resultParse = shader->parse(GetDefaultResources(), 460, false, messages, includer);

	program->addShader(shader);

	if (!resultParse)
	{
		std::string infoLog = shader->getInfoLog();
		std::string debugLog = shader->getInfoDebugLog();

		ParseErrors(shader->getInfoLog(), errors);
		return false;
	}

	program->link(messages);
	program->mapIO();

	auto intermediate = shader->getIntermediate();


	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;

	if (isDebug)
	{
		std::string allSourceText = "";
		for (auto s : source_strings)
		{
			allSourceText += s;
			allSourceText += '\n';
		}

		intermediate->addSourceText(allSourceText.c_str(), allSourceText.size());
		spvOptions.generateDebugInfo = true;
		spvOptions.emitNonSemanticShaderDebugInfo = true;
		spvOptions.emitNonSemanticShaderDebugSource = true;
	}
	else
	{
		spvOptions.stripDebugInfo = true;
	}

	spvOptions.disableOptimizer = isDebug;
	spvOptions.optimizeSize = !isDebug;
	spvOptions.disassemble = true;
	spvOptions.validate = false; //validate only

	std::vector<uint32_t> spirv;
	glslang::GlslangToSpv(*intermediate, spirv, &logger, &spvOptions);

	src->cachedSPIRV = spirv;

	int textSize = spirv.size() * 21;
	std::string output = "";
	output.reserve(textSize);
	const u8* spirvdata = (const u8*)spirv.data();
	int curlinesize = 0;

	for (int i = 0; i < spirv.size() * 4; ++i)
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