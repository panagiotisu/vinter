#pragma once

#include <filesystem>

struct SDL_GPUShader;
struct SDL_GPUDevice;

namespace vn {
    class Shader {
    public:
        enum class Stage : std::uint8_t {
            Fragment,
            Vertex,
            Compute,
        };

    public:
        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~Shader();

        [[nodiscard]]
        static auto from_spirv(
            SDL_GPUDevice* gpu_handle,
            const unsigned char* spirv_bytecode,
            unsigned int spirv_bytecode_size,
            Stage stage
        ) -> Shader;

        [[nodiscard]]
        static auto
        from_hlsl(SDL_GPUDevice* gpu_handle, const std::string& hlsl_source, Stage stage) -> Shader;

        [[nodiscard]]
        static auto from_file(SDL_GPUDevice* gpu_handle, const std::filesystem::path& filepath)
            -> Shader;

        [[nodiscard]]
        auto get_native_handle() const noexcept -> SDL_GPUShader*;

    private:
        Shader(SDL_GPUDevice* gpu_handle, SDL_GPUShader* shader_handle, Stage stage);

        [[nodiscard]]
        static constexpr auto to_sdl_shadercross_shader_stage(Stage stage) -> std::uint32_t;

    private:
        Stage m_stage {};

        SDL_GPUShader* m_handle {};
        SDL_GPUDevice* m_gpu_handle {};
    };
} // namespace vn