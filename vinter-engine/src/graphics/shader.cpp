#include "vinter/graphics/shader.hpp"

#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include "vinter/logger.hpp"

namespace vn {
    Shader::Shader(SDL_GPUDevice* gpu_handle, SDL_GPUShader* shader_handle, Stage stage)
        : m_gpu_handle(gpu_handle)
        , m_handle(shader_handle)
        , m_stage(stage) {
    }

    Shader::Shader(Shader&& other) noexcept
        : m_stage(other.m_stage)
        , m_handle(std::exchange(other.m_handle, nullptr))
        , m_gpu_handle(other.m_gpu_handle) {
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (m_handle != nullptr) {
                SDL_ReleaseGPUShader(m_gpu_handle, m_handle);
            }
            m_stage = other.m_stage;
            m_handle = std::exchange(other.m_handle, nullptr);
            m_gpu_handle = other.m_gpu_handle;
        }
        return *this;
    }

    Shader::~Shader() {
        if (m_handle != nullptr) {
            SDL_ReleaseGPUShader(m_gpu_handle, m_handle);
        }
    }

    auto Shader::from_spirv(
        SDL_GPUDevice* gpu_handle,
        const unsigned char* spirv_bytecode,
        unsigned int spirv_bytecode_size,
        Stage stage
    ) -> Shader {
        VN_ASSERT(stage != Stage::Compute, "Compute shaders are not supported at this point.");

        SDL_ShaderCross_GraphicsShaderMetadata* meta = SDL_ShaderCross_ReflectGraphicsSPIRV(
            spirv_bytecode, spirv_bytecode_size, 0
        );
        VN_ASSERT(meta != nullptr, "Failed to reflect SPIR-V shader.");

        const SDL_ShaderCross_SPIRV_Info spirv_info = {
            .bytecode = spirv_bytecode,
            .bytecode_size = spirv_bytecode_size,
            .entrypoint = "main",
            .shader_stage =
                static_cast<SDL_ShaderCross_ShaderStage>(to_sdl_shadercross_shader_stage(stage))
        };

        SDL_GPUShader* shader_handle = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            gpu_handle, &spirv_info, &meta->resource_info, 0
        );
        SDL_free(meta);
        VN_ASSERT(shader_handle != nullptr, "Failed to compile graphics shader from SPIR‑V.");

        return { gpu_handle, shader_handle, stage };
    }

    auto Shader::from_hlsl(SDL_GPUDevice* gpu_handle, const std::string& hlsl_source, Stage stage)
        -> Shader {
        const SDL_ShaderCross_HLSL_Info hlsl_info = {
            .source = hlsl_source.c_str(),
            .entrypoint = "main",
            .include_dir = nullptr,
            .defines = nullptr,
            .shader_stage =
                static_cast<SDL_ShaderCross_ShaderStage>(to_sdl_shadercross_shader_stage(stage)),
            .props = 0,
        };

        std::size_t spirv_bytecode_size = 0;
        void* raw = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlsl_info, &spirv_bytecode_size);
        auto* spirv_bytecode = static_cast<unsigned char*>(raw);

        Shader shader = from_spirv(gpu_handle, spirv_bytecode, spirv_bytecode_size, stage);

        SDL_free(raw);

        return shader;
    }

    auto Shader::from_file(SDL_GPUDevice* gpu_handle, const std::filesystem::path& filepath)
        -> Shader {
        const std::string filename = filepath.filename().string();

        // Infer stage
        Stage stage {};
        if (filename.contains(".frag")) {
            stage = Stage::Fragment;
        } else if (filename.contains(".vert")) {
            stage = Stage::Vertex;
        } else if (filename.contains(".comp")) {
            stage = Stage::Compute;
        } else {
            VN_FATAL("Could not infer shader stage from shader file {}", filepath.string());
        }

        if (filename.contains(".spv")) {
            // Read binary file
            std::ifstream file(filepath, std::ios::binary | std::ios::ate);
            if (!file) {
                VN_FATAL("Failed to open SPIR‑V shader file: {}", filepath.string());
            }

            auto size = file.tellg();
            std::vector<unsigned char> bytecode(static_cast<size_t>(size));

            file.seekg(0);
            file.read(reinterpret_cast<char*>(bytecode.data()), size);

            if (!file) {
                VN_FATAL("Failed to read SPIR‑V shader file: {}", filepath.string());
            }

            return from_spirv(
                gpu_handle, bytecode.data(), static_cast<unsigned int>(bytecode.size()), stage
            );
        }

        if (filename.contains(".hlsl")) {
            std::ifstream file(filepath);
            if (!file) {
                VN_FATAL("Failed to open HLSL shader file: {}", filepath.string());
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            if (!file && !file.eof()) {
                VN_FATAL("Failed to read HLSL shader file: {}", filepath.string());
            }

            return from_hlsl(gpu_handle, buffer.str(), stage);
        }

        VN_FATAL("Could not infer shader format from shader file {}", filepath.string());
    }

    auto Shader::get_native_handle() const noexcept -> SDL_GPUShader* {
        return m_handle;
    }

    constexpr auto Shader::to_sdl_shadercross_shader_stage(Stage stage) -> std::uint32_t {
        switch (stage) {
            case Stage::Fragment: return SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
            case Stage::Vertex: return SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
            case Stage::Compute: return SDL_SHADERCROSS_SHADERSTAGE_COMPUTE;
        }
    }
} // namespace vn