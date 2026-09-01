#include "vinter/graphics/sprite.hpp"

#include "vinter/ecs/database.hpp"
#include "vinter/graphics/renderer.hpp"
#include "vinter/spatial/geometry.hpp"
#include "vinter/spatial/transform.hpp"

namespace vn::ecs {
    Sprite Sprite::create(Texture texture, std::size_t frames_per_row, std::size_t frames_per_col) {
        return {
            .texture = texture,
            .frame_size = { static_cast<float>(texture.get_width())
                                / static_cast<float>(frames_per_row),
                            static_cast<float>(texture.get_height())
                                / static_cast<float>(frames_per_col) },
            .frames_per_row = frames_per_row,
            .frames_per_col = frames_per_col,
        };
    }

    void SpriteAnimator::add(
        const std::string& animation_name,
        std::size_t start_frame,
        std::size_t end_frame,
        float fps,
        bool looping
    ) {
        animations.insert_or_assign(
            animation_name,
            SpriteAnimation {
                .fps = fps,
                .start_frame = start_frame,
                .end_frame = end_frame,
                .looping = looping,
            }
        );
    }

    void SpriteAnimator::play(Sprite& sprite, const std::string& animation_name) {
        auto it = animations.find(animation_name);
        if (it == animations.end()) {
            return;
        }
        SpriteAnimation* desired_animation = &it->second;

        if (current_animation == nullptr || current_animation != desired_animation) {
            current_animation = desired_animation;
            sprite.current_frame = current_animation->start_frame;
            current_animation->frame_time_elapsed = 0.f;
        }
    }

    void SpriteAnimator::remove(const std::string& animation_name) {
        auto it = animations.find(animation_name);
        if (it == animations.end()) {
            return;
        }
        if (current_animation == &it->second) {
            current_animation = nullptr;
        }
        animations.erase(it);
    }

    SpriteSystem::SpriteSystem(Database& database, Renderer& renderer)
        : ISystem(database)
        , m_renderer(renderer) {
    }

    void SpriteSystem::update(float delta) {
        m_database.query<Sprite, SpriteAnimator>().for_each([&](Sprite& sprite,
                                                                SpriteAnimator& animator) {
            float frame_duration = 1.f / animator.current_animation->fps;
            animator.current_animation->frame_time_elapsed += delta * animator.speed_scale;

            // Only change frame if current frame is finished.
            if (animator.current_animation->frame_time_elapsed < frame_duration) {
                return;
            }
            sprite.current_frame += 1;
            animator.current_animation->frame_time_elapsed = 0.f;

            if (sprite.current_frame <= animator.current_animation->end_frame) {
                return;
            }
            // If animation reached the last frame and looping is enabled, reset to first.
            if (animator.current_animation->looping) {
                sprite.current_frame = animator.current_animation->start_frame;
            } else {
                // Latch to last frame if not looping.
                sprite.current_frame = animator.current_animation->end_frame;
            }
        });
    }

    void SpriteSystem::render() {
        m_database.query<Sprite, Transform>().for_each([&](const Sprite& sprite,
                                                           const Transform& transform) {
            const auto col = sprite.current_frame % sprite.frames_per_row;
            const auto row = sprite.current_frame / sprite.frames_per_row;
            const AABB src_aabb = {
                .position = { static_cast<float>(col) * sprite.frame_size.x,
                              static_cast<float>(row) * sprite.frame_size.y },
                .size = sprite.frame_size,
            };

            const AABB dest_aabb = {
                .position = { transform.global.position.x, transform.global.position.y },
                .size = src_aabb.size * transform.global.scale,
            };

            const glm::vec2 pivot = { dest_aabb.size.x / 2, dest_aabb.size.y / 2 };

            m_renderer.draw_texture(
                sprite.texture, src_aabb, dest_aabb, transform.global.rotation, pivot, sprite.flip
            );
        });
    }
} // namespace vn::ecs
