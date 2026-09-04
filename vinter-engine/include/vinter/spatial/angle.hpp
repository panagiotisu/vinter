#pragma once

#include <numbers>

#include <glm/glm.hpp>

namespace vn {
    /**
     * @brief Angle object to conveniently switch between degree and
     * radians representations.
     */
    class Angle {
    public:
        /**
         * @brief Constructs an Angle object from a degrees float literal.
         *
         * @param degrees The degrees float literal.
         * @return constexpr Angle The constructed Angle object.
         */
        [[nodiscard]]
        static constexpr Angle from_degrees(float degrees) {
            return Angle(degrees * Pi / 180.f);
        }

        /**
         * @brief Constructs an Angle object from a radians float literal.
         *
         * @param degrees The radians float literal.
         * @return constexpr Angle The constructed Angle object.
         */
        [[nodiscard]]
        static constexpr Angle from_radians(float radians) {
            return Angle(radians);
        }

        /**
         * @brief Constructs an Angle object from a 2D vector object. Represents the angle formed
         * between the vector and the x-axis.
         *
         * @param vector The vector whose angle with the x-axis forms the Angle object.
         * @return constexpr Angle The constructed Angle object.
         */
        [[nodiscard]]
        static constexpr Angle from_vector(glm::vec2 vector) {
            return Angle(std::atan2(vector.y, vector.x));
        }

    public:
        /**
         * @brief Returns the Angle represented in degrees.
         *
         * @return constexpr float The angle represented in degrees.
         */
        [[nodiscard]]
        constexpr float as_degrees() const {
            return m_radians * (180.f / Pi);
        }

        /**
         * @brief Returns the Angle represented in radians.
         *
         * @return constexpr float The angle represented in radians.
         */
        [[nodiscard]]
        constexpr float as_radians() const {
            return m_radians;
        }

        /**
         * @brief Wraps the angle into the range: [-π, π) radians.
         *
         * @return constexpr Angle The signed wrapped angle.
         */
        [[nodiscard]]
        constexpr Angle wrap_signed() const {
            float wrapped = std::fmod(m_radians + Pi, Tau);
            if (wrapped < 0) {
                wrapped += Tau;
            }
            return Angle(wrapped - Pi);
        }

        /**
         * @brief Wraps the angle into the range: [0, 2π) radians.
         *
         * @return constexpr Angle The unsigned wrapped angle.
         */
        [[nodiscard]]
        constexpr Angle wrap_unsigned() const {
            float wrapped = std::fmod(m_radians, Tau);
            if (wrapped < 0) {
                wrapped += Tau;
            }
            return Angle(wrapped);
        }

    private:
        constexpr explicit Angle(float radians) : m_radians(radians) {
        }

    private:
        static constexpr float Pi { std::numbers::pi_v<float> };
        static constexpr float Tau { 2 * Pi };

        float m_radians {};
    };

    constexpr Angle operator-(Angle angle) {
        return Angle::from_radians(-angle.as_radians());
    }

    constexpr Angle operator+(Angle lhs, Angle rhs) {
        return Angle::from_radians(lhs.as_radians() + rhs.as_radians());
    }

    constexpr Angle& operator+=(Angle& lhs, Angle rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    constexpr Angle operator-(Angle lhs, Angle rhs) {
        return Angle::from_radians(lhs.as_radians() - rhs.as_radians());
    }

    constexpr Angle& operator-=(Angle& lhs, Angle rhs) {
        lhs = lhs - rhs;
        return lhs;
    }

    constexpr Angle operator*(Angle angle, const float scalar) {
        return Angle::from_radians(angle.as_radians() * scalar);
    }

    constexpr Angle operator*(const float scalar, Angle angle) {
        return angle * scalar;
    }

    constexpr Angle& operator*=(Angle& angle, const float scalar) {
        angle = angle * scalar;
        return angle;
    }

    constexpr Angle operator/(Angle angle, const float scalar) {
        return Angle::from_radians(angle.as_radians() / scalar);
    }

    constexpr Angle& operator/=(Angle& angle, const float scalar) {
        angle = angle / scalar;
        return angle;
    }

    constexpr float operator/(Angle lhs, Angle rhs) {
        return lhs.as_radians() / rhs.as_radians();
    }

    constexpr Angle operator%(Angle lhs, Angle rhs) {
        float mod = std::fmod(lhs.as_radians(), rhs.as_radians());
        if (mod < 0) {
            mod += rhs.as_radians();
        }
        return Angle::from_radians(mod);
    }

    constexpr Angle& operator%=(Angle& lhs, Angle rhs) {
        lhs = lhs % rhs;
        return lhs;
    }

    constexpr bool operator==(Angle lhs, Angle rhs) {
        constexpr float Epsilon = 1e-6;
        return std::fabs((lhs - rhs).wrap_signed().as_radians()) < Epsilon;
    }

    constexpr bool operator!=(Angle lhs, Angle rhs) {
        return !(lhs == rhs);
    }

    constexpr bool operator<(Angle lhs, Angle rhs) {
        return lhs.as_radians() < rhs.as_radians();
    }

    constexpr bool operator>(Angle lhs, Angle rhs) {
        return rhs < lhs;
    }

    constexpr bool operator<=(Angle lhs, Angle rhs) {
        return !(rhs < lhs);
    }

    constexpr bool operator>=(Angle lhs, Angle rhs) {
        return !(lhs < rhs);
    }

    constexpr Angle operator""_deg(const long double angle) {
        return Angle::from_degrees(static_cast<float>(angle));
    }

    constexpr Angle operator""_deg(const unsigned long long int angle) {
        return Angle::from_degrees(static_cast<float>(angle));
    }

    constexpr Angle operator""_rad(const long double angle) {
        return Angle::from_radians(static_cast<float>(angle));
    }

    constexpr Angle operator""_rad(const unsigned long long int angle) {
        return Angle::from_radians(static_cast<float>(angle));
    }
} // namespace vn