#pragma once

#include <string>

#include <glm/glm.hpp>

#include "vinter/ecs/system.hpp"
#include "vinter/graphics/texture.hpp"

namespace vn {
    struct Renderer;
} // namespace vn

namespace vn::ecs {
    class Database;

    struct Sprite {
        Texture texture {};
        glm::vec2 frame_size {};
        std::size_t frames_per_row { 1 };
        std::size_t frames_per_col { 1 };
        std::size_t current_frame {};
        glm::bvec2 flip {};

        [[nodiscard]]
        static Sprite create(
            Texture texture = {},
            std::size_t frames_per_row = 1,
            std::size_t frames_per_col = 1
        );
    };

    struct SpriteAnimation {
        float frame_time_elapsed {};
        float fps { 3 };
        std::size_t start_frame {};
        std::size_t end_frame {};
        bool looping {};

        friend bool operator==(const SpriteAnimation& lhs, const SpriteAnimation& rhs) {
            return std::abs(lhs.frame_time_elapsed - rhs.frame_time_elapsed) < 1e-6
                   && lhs.start_frame == rhs.start_frame && lhs.end_frame == rhs.end_frame
                   && lhs.looping == rhs.looping;
        }
    };

    struct SpriteAnimator {
        std::unordered_map<std::string, SpriteAnimation> animations {};
        SpriteAnimation* current_animation {};
        float speed_scale = 1.f;

        void
        add(const std::string& name,
            std::size_t start_frame = 0,
            std::size_t end_frame = 0,
            float fps = 3,
            bool looping = false);

        void play(Sprite& sprite, const std::string& animation_name);

        void remove(const std::string& name);
    };

    class SpriteSystem : public ISystem {
    public:
        explicit SpriteSystem(Database& database, Renderer& renderer);

        void update(float delta) override;
        void render() override;

    private:
        Renderer& m_renderer;
    };

} // namespace vn::ecs
