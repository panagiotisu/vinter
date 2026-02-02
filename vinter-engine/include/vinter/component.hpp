#pragma once

namespace vn {
    class GameObject;

    class Component {
        friend class GameObject;

    public:
        virtual ~Component() = default;

        [[nodiscard]] GameObject* get_owner() const noexcept { return m_owner; }

        template<typename T>
        T* get_component();

        template<typename T>
        [[nodiscard]] bool has_component() const;

        template<typename T>
        void remove_component() const;

        virtual void on_register() {}
        virtual void ready() {}
        virtual void update(float delta) {}
        virtual void on_remove() {}

    private:
        GameObject* m_owner = nullptr;
    };
} // vn