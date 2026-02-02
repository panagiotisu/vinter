#pragma once

namespace vn {
    template<typename T>
    T* Component::get_component() {
        return m_owner ? m_owner->get_component<T>() : nullptr;
    }

    template<typename T>
    bool Component::has_component() const {
        return m_owner && m_owner->has_component<T>();
    }

    template<typename T>
    void Component::remove_component() const {
        if (m_owner) {
            m_owner->remove_component<T>();
        }
    }
} // vn