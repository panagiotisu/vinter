#pragma once

#include <vinter/app.hpp>

struct Player {};

enum class Affiliation : std::uint8_t {
    Player,
    Friendly,
    Enemy,
    Neutral,
};

struct Transform2D {
    struct Data {
        glm::vec2 position {};
        glm::vec2 scale { 1.f, 1.f };
        float rotation {};
    };

    Data local {};
    Data global {};
    vn::Entity parent {};
};

struct LinearKinematics2D {
    glm::vec2 velocity {};
    glm::vec2 acceleration_direction {};
    float max_speed { 400.f };
    float acceleration_coefficient { 5.f };
};

struct Health {
    float max_health { 100.f };
    float regeneration { 1.f };
    float current_health { 1.f };
};

struct Circle {
    vn::Color color { vn::colors::White };
    float radius { 1.f };
};

struct AABB {
    vn::Color color { vn::colors::White };
    glm::vec2 size { 1.f, 1.f };
};

struct Sprite2D {
    // vn::Texture texture {};
    vn::Color tint { vn::colors::White };
    glm::vec2 frame_size {};
    std::size_t frames_per_row { 1 };
    std::size_t frames_per_col { 1 };
    std::size_t current_frame {};
    bool flip_h = { false };
    bool flip_v { false };
};

struct SpriteAnimation {
    float frame_time_elapsed {};
    float fps { 3 };
    std::size_t start_frame {};
    std::size_t end_frame {};
    bool looping { false };
};

struct SpriteAnimator2D {
    std::unordered_map<std::string, SpriteAnimation> animations {};
    SpriteAnimation* current_animation {};
    float speed_scale { 1.f };
};