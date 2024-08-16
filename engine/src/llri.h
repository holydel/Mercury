#pragma once

#include "mercury_api.h"
#include "mercury_shader.h"
#include "mercury_material.h"
#include "mercury_buffer.h"

namespace llri
{
	bool initialize();
	void shutdown();

	bool update();

	struct CommandList
	{
		void* impl;			
	};

	CommandList BeginOneTimeSubmitCommandList();
	void EndOneTimeSubmitCommandList(CommandList clist);

	mercury::Shader create_shader_from_bytecode(mercury::Shader::ByteCode bc);
	mercury::Material create_material(mercury::Material::Desc);
	mercury::Buffer create_buffer(mercury::u64 size, mercury::Buffer::HeapType heapType);
	void* buffer_mapped_data(mercury::u32 handle);
}