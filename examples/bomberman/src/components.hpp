#pragma once

#include <vinter/app.hpp>

struct Player {};

enum class Affiliation : std::uint8_t {
    Player,
    Friendly,
    Enemy,
    Neutral,
};

struct LinearKinematics {
    glm::vec2 velocity {};
    glm::vec2 acceleration_direction {};
    float max_speed { 400.f };
    float acceleration_coefficient { 5.f };
};

struct SpriteDirector {
    enum class Cardinal : std::uint8_t {
        Up,
        UpRight,
        Right,
        DownRight,
        Down,
    };

    enum class FacePattern : std::uint8_t {
        Linear,
        Square,
        Rhombus,
        Hexagon,
        Octagon,
    };

    Cardinal cardinal { Cardinal::Right };
    FacePattern face_pattern { FacePattern::Linear };
    glm::vec2 face_direction { 1.f, 0.f };
};