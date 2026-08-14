#pragma once

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "vinter/containers/sparse_set.hpp"
#include "vinter/containers/type_list.hpp"
#include "vinter/utils/demangle.hpp"

namespace vn {
    template <typename... Components>
    class View;
    class ECS;

    class Entity {
        friend class ECS;

        template <typename... Components>
        friend class View;

    public:
        // Entity::Id uses the first bits for versioning, and any subsequent bits for the actual
        // entity Id index.
        // Entity::Id = [ Entity::Version | Entity::Index ]
        using Id = std::uint64_t;
        using Version = std::uint16_t;
        using Index = std::uint64_t;

        static constexpr std::uint32_t IdBits { sizeof(Id) * 8 };
        static constexpr std::uint32_t VersionBits { 12 };
        static constexpr std::uint32_t IndexBits { IdBits - VersionBits };

        static constexpr Id IndexMask { (static_cast<Id>(1) << IndexBits) - 1 };
        static constexpr Id VersionMask { ~IndexMask };
        static constexpr Version MaxVersion { (static_cast<Version>(1) << VersionBits) - 1 };
        static constexpr Id Null { std::numeric_limits<Id>::max() };

    public:
        Entity() = default;

        [[nodiscard]]
        Id id() const noexcept {
            return m_id;
        };

        [[nodiscard]]
        Version version() const noexcept {
            return static_cast<Version>((m_id & VersionMask) >> IndexBits);
        }

        [[nodiscard]]
        Index index() const noexcept {
            return static_cast<Index>(m_id & IndexMask);
        }

        [[nodiscard]]
        bool is_null() const noexcept {
            return m_id == Null;
        }

        constexpr explicit operator bool() const {
            return !is_null();
        }

        friend constexpr bool operator==(Entity lhs, Entity rhs) {
            return lhs.m_id == rhs.m_id;
        }

        friend constexpr bool operator!=(Entity lhs, Entity rhs) {
            return !(lhs == rhs);
        }

    private:
        Id m_id {};

        Entity(Version version, Index index) : m_id(encode(version, index)) {
            static_assert(IdBits == sizeof(Id) * 8);
            static_assert(VersionBits <= sizeof(Version) * 8);
            static_assert(IndexBits <= sizeof(Index) * 8);
        }

        static constexpr Id encode(Version version, Index index) {
            return ((static_cast<Id>(version) << IndexBits) & VersionMask)
                   | (static_cast<Id>(index) & IndexMask);
        }
    };

    class ECS {
        template <typename... Components>
        friend class View;

        // clang-format off
        #define VN_ECS_ASSERT_ENTITY_VALID(entity)                \
            VN_ASSERT(                                            \
                (entity).id() != Entity::Null,                    \
                "Entity::Null cannot be operated on by the ECS."  \
            );                                                    \
            VN_ASSERT(                                            \
                (entity).index() < m_entity_infos.size(),         \
                "Entity::Index out of bounds: {}", (entity).id()  \
            )

        #define VN_ECS_ASSERT_ENTITY_ALIVE(entity)                         \
            VN_ASSERT(                                                     \
                is_alive(entity),                                          \
                "Attempting to access inactive entity: {}", (entity).id()  \
            )
        // clang-format on

    public:
        /**
         * @brief Max amount of entities alive at once. Entity.id() should not exceed
         * this number.
         */
        static constexpr std::size_t MaxEntities {
            (static_cast<std::size_t>(1) << Entity::IndexBits) - 1
        };

        /**
         * @brief Should be a multiple of 32 (4 bytes), since bitset overallocates by
         * 4 bytes each time.
         */
        static constexpr std::size_t MaxComponents { 1024 };

        /**
         * @brief Each bit in the mask represents a component type:
         * '1': component exists on entity
         * '0': component doesn't exist on entity
         */
        using ComponentMask = std::bitset<MaxComponents>;

        ECS() {
            VN_INFO("Created ECS.");
        }

        ~ECS() {
            VN_INFO("Destroyed ECS.");
        }

        /**
         * @brief Creates an entity and returns its handle.
         *
         * @param name Name of the entity.
         * @return Entity handle.
         */
        Entity create_entity(std::string name = {}) {
            Entity::Index index {};

            // Either spawn a new entity index or recycle an existing one.
            if (m_available_entities.size() == 0) {
                VN_ASSERT(
                    m_max_entity_index < MaxEntities, "Entity limit of {} exceeded.", MaxEntities
                );
                index = m_max_entity_index++;
                m_entity_infos.push_back(EntityInfo {});
            } else {
                index = m_available_entities.back();
                m_available_entities.pop_back();
            }

            if (!name.empty()) {
                m_entity_names.set(index, name);
            }

            Entity entity(m_entity_infos[index].version, index);
            VN_DEBUG("Created {}", get_entity_info_string(entity));
            return entity;
        }

        /**
         * @brief Returns an info string of the name, index and version of entity.
         *
         * @param entity The entity whose info is queried.
         * @return std::string The info string of entity.
         */
        [[nodiscard]]
        std::string get_entity_info_string(Entity entity) const {
            return std::format(
                "Entity '{}' : [Index: {} | Version: {}]",
                get_entity_name(entity),
                entity.index(),
                entity.version()
            );
        }

        /**
         * @brief Returns the name of an entity if it has one,
         *  otherwise defaults to the name: "Entity {Id}""
         *
         * @param entity Entity whose name is queried.
         * @return std::string Name of the entity.
         */
        [[nodiscard]]
        std::string get_entity_name(Entity entity) const {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            const auto* name = m_entity_names.get(entity.index());
            if (name != nullptr) {
                return *name;
            }

            return std::format("Entity {}", entity.id());
        }

        /**
         * @brief Checks whether an entity is active, returning 'true' if it is, or 'false' if it
         * isn't.
         *
         * @param entity Entity whose active status is queried.
         * @return true If entity is active.
         * @return false If entity is inactive
         *   (entity.id() == Entity::Null || entity.index() >= m_entity_infos.size()).
         */
        [[nodiscard]]
        bool is_alive(Entity entity) const {
            if (entity.id() == Entity::Null || entity.index() >= m_entity_infos.size()) {
                return false;
            }
            return m_entity_infos[entity.index()].version == entity.version();
        }

        /**
         * @brief Deletes an active entity and its associated components.
         *
         * @note Overwrites the given entity to Entity::Null.
         *
         * @param entity The entity to be deactivated.
         */
        void delete_entity(Entity& entity) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            const ComponentMask& mask = get_entity_component_mask(entity);

            const Entity::Index index = entity.index();

            // clang-format off
            #ifndef NDEBUG
                const std::string entity_info = get_entity_info_string(entity);
            #endif
            // clang-format on

            // Destroy associated components.
            for (std::size_t i = 0; i < m_component_pools.size(); i++) {
                if (static_cast<int>(mask[i]) == 1 && m_component_pools[i] != nullptr) {
                    m_component_pools[i]->unset(index);
                }
            }

            // Reset entity info and increment version on deactivation, allowing the tracking of
            // active / inactive entities.
            m_entity_infos[index].component_mask = {};
            m_entity_infos[index].version = get_next_version(m_entity_infos[index].version);

            m_entity_names.unset(index);
            m_available_entities.push_back(index);

            entity.m_id = Entity::Null;
            VN_DEBUG("Deleted {}", entity_info);
        }

        /**
         * @brief Registers a component and creates a component pool for it.
         *
         * @note Called automatically on component addition.
         *
         * @tparam T The component type.
         */
        template <typename T>
        void register_component() {
            const std::size_t component_index = get_component_index<T>();
            VN_ASSERT(
                component_index < MaxComponents,
                "Exceeded maximum allowed number of registered components."
            );

            if (component_index >= m_component_pools.size()) {
                m_component_pools.resize(component_index + 1);
            }

            VN_ASSERT(
                m_component_pools[component_index] == nullptr,
                "Attempted to register component '{}' twice.",
                demangle(typeid(T).name())
            );

            m_component_pools[component_index] = std::make_unique<SparseSet<T>>();
            VN_DEBUG("Registered Component '{}'.", demangle(typeid(T).name()));
        }

        /**
         * @brief Attaches a component to an entity using forwarding, and retrieves a reference.
         *
         * Example:
         * Transform& transform = ecs.add<Transform>(player);
         *
         * @note If the component already exists, its data are overriden.
         *
         * @tparam T The Component type.
         * @param entity The entity the component is attaching to.
         * @param component The component the entity is attached to.
         * @return T& The attached component reference.
         */
        template <typename T>
        T& add(Entity entity, T&& component = {}) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            SparseSet<T>& component_pool = get_component_pool<T>();
            const Entity::Index entity_index = entity.index();

            // If component already exists, overwrite its data with new data.
            if (component_pool.get(entity_index)) {
                return *component_pool.set(entity_index, std::forward<T>(component));
            }

            ComponentMask& component_mask = get_entity_component_mask(entity);
            set_component_mask_bit<T>(component_mask, true);

            VN_DEBUG(
                "Attached component '{}' to {},",
                demangle(typeid(T).name()),
                get_entity_info_string(entity)
            );
            return *component_pool.set(entity_index, std::forward<T>(component));
        }

        /**
         * @brief Retrieves a reference to the specified component type for a given entity.
         *
         * Example:
         * Transform& transform = ecs.get<Transform>(player);
         *
         * @tparam T The component type.
         * @param entity The entity queried for the component.
         * @return T& The component reference.
         */
        template <typename T>
        [[nodiscard]]
        T& get(Entity entity) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            const SparseSet<T>& component_pool = get_component_pool<T>();
            T* component = component_pool.get(entity.index());

            VN_ASSERT(
                component != nullptr,
                "{} missing component in '{}' pool.",
                get_entity_info_string(entity),
                demangle(typeid(T).name())
            );
            return *component;
        }

        /**
         * @brief Retrieves a nullable pointer to the specified component type for a given entity.
         *
         * Example:
         * Transform* transform = ecs.get_ptr<Transform>(player);
         *
         * @tparam T The component type.
         * @param entity The entity queried for the component.
         * @return T* The component pointer.
         */
        template <typename T>
        T* get_ptr(Entity entity) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            const SparseSet<T>& component_pool = get_component_pool<T>();
            return component_pool.get(entity.index());
        }

        /**
         * @brief Removes a component from an entity.
         *
         * Example:
         * get_ecs().remove<Transform>(player);
         *
         * @tparam T
         * @param entity
         */
        template <typename T>
        void remove(Entity entity) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            const SparseSet<T>& component_pool = get_component_pool<T>();
            const Entity::Index entity_index = entity.index();

            if (component_pool.get(entity_index) == nullptr) {
                return;
            }

            ComponentMask& component_mask = get_entity_component_mask(entity);
            set_component_mask_bit<T>(component_mask, false);

            component_pool.unset(entity_index);
            VN_DEBUG(
                "Removed component '{}' from {},",
                demangle(typeid(T).name()),
                get_entity_info_string(entity)
            );
        }

        /**
         * @brief Returns true if the entity has ALL the specified components attached to it.
         *
         * @tparam Ts The types of the specified components.
         * @param entity The entity queried for components.
         * @return true If the enttiy has ALL the specified component types.
         * @return false If the entity is missing one or more of the component types.
         */
        template <typename... Ts>
        [[nodiscard]]
        bool has(Entity entity) {
            if (!is_alive(entity)) {
                return false;
            }
            auto& component_mask = get_entity_component_mask(entity);
            return (get_component_mask_bit<Ts>(component_mask) && ...);
        }

        /**
         * @brief Returns true if the entity has at least one of the specified components attached
         * to it.
         *
         * @tparam Ts The types of the specified components.
         * @param entity The entity queried for components.
         * @return true If the enttiy has ANY the specified component types.
         * @return false If the entity is missing all of the component types.
         */
        template <typename... Ts>
        [[nodiscard]]
        bool has_any(Entity entity) {
            return (has<Ts>(entity) || ...);
        }

        template <typename... Components>
        View<Components...> view() {
            return View<Components...> { this };
        }

        [[nodiscard]]
        std::size_t pool_count() const {
            return m_component_pools.size();
        }

        void print_entity_components(Entity entity) {
            VN_ECS_ASSERT_ENTITY_VALID(entity);
            VN_ECS_ASSERT_ENTITY_ALIVE(entity);

            std::string result = get_entity_info_string(entity);
            result += " Components:";

            const ComponentMask& component_mask = get_entity_component_mask(entity);
            bool first = true;

            for (std::size_t i = 0; i < MaxComponents; i++) {
                if (component_mask[i]) {
                    if (!first) {
                        result += "\n";
                    }

                    result += m_component_names[i];
                    first = false;
                }
            }
            VN_DEBUG("{}", result);
        }

        /**
         * @brief Erases all ECS data, resetting its internal state.
         */
        void reset() {
            m_available_entities.clear();
            m_entity_names.clear();
            m_component_pools.clear();
            m_max_entity_index = 0;
        }

    private:
        /**
         * @brief Returns the next unused component index.
         *
         * Don't call directly for component type lookup, since each call generates a new
         * index. It is intended to be used only during the first component initialization
         * inside get_component_index
         *
         * @param type_name The name of the next component type.
         * @return std::size_t The next component type index.
         */
        static std::size_t next_component_index(const std::string& type_name) {
            static std::size_t index = 0;
            m_component_names.push_back(type_name);
            return index++;
        }

        /**
         * @brief Returns a unique identifier (index) for each type, used to index component pools.
         *
         * @note Calling this twice for the same type returns the same index.
         *
         * This works since it's a static templated function, so the compiler will generate a
         * unique version of this function per type when the program is compiled.
         * Since it's static, all ECS instances share the same index for each component type.
         *
         * @tparam T The component type.
         * @return std::size_t The component index.
         */
        template <typename T>
        [[nodiscard]]
        static std::size_t get_component_index() {
            static std::size_t index = next_component_index(typeid(T).name());
            return index;
        }

        /**
         * @brief Same as get_component_index, but registers the component if it does not exist in
         * the ECS yet.
         *
         * @tparam T The component type.
         * @return std::size_t The component index
         */
        template <typename T>
        [[nodiscard]]
        std::size_t get_or_register_component_index() {
            std::size_t component_index = get_component_index<T>();

            if (component_index >= m_component_pools.size()
                || m_component_pools[component_index] == nullptr) {
                register_component<T>();
            }

            VN_ASSERT(
                component_index < m_component_pools.size(),
                "Component index out of bounds for component '{}'",
                demangle(typeid(T).name())
            );

            return component_index;
        }

        /**
         * @brief Retrieves an uncasted pointer to a component pool of type T.
         *
         * @tparam T The component type.
         * @return ISparseSet* The uncasted pointer to a component pool of type T.
         */
        template <typename T>
        [[nodiscard]]
        ISparseSet* get_component_pool_ptr() {
            size_t index = get_or_register_component_index<T>();
            return m_component_pools[index].get();
        }

        /**
         * @brief Retrieves a reference to a component pool of type T.
         *
         * @tparam T The component type.
         * @return SparseSet<T>& The component pool of type T reference.
         */
        template <typename T>
        [[nodiscard]]
        SparseSet<T>& get_component_pool() {
            ISparseSet* base_ptr = get_component_pool_ptr<T>();
            auto* pool = static_cast<SparseSet<T>*>(base_ptr);
            return *pool;
        }

        template <typename T>
        static void set_component_mask_bit(ComponentMask& mask, bool value) {
            std::size_t bit_position = get_component_index<T>();
            mask[bit_position] = value;
        }

        template <typename T>
        [[nodiscard]]
        static bool get_component_mask_bit(ComponentMask& mask) {
            std::size_t bit_position = get_component_index<T>();
            return mask[bit_position];
        }

        [[nodiscard]]
        ComponentMask& get_entity_component_mask(Entity entity) {
            return m_entity_infos[entity.index()].component_mask;
        }

        /**
         * @brief Generates a component mask given component types.
         *
         * generate_component_mask<A,B,D>() -> D1 B0 C1 A1 (1011)
         *
         * @tparam T The component type.
         * @return ComponentMask The generated component mask bitset.
         */
        template <typename... T>
        [[nodiscard]]
        static ComponentMask generate_component_mask() {
            ComponentMask mask {};
            (set_component_mask_bit<T>(mask, true), ...);
            return mask;
        }

        [[nodiscard]]
        static Entity::Version get_next_version(Entity::Version current_version) {
            return static_cast<Entity::Version>((current_version + 1) & Entity::MaxVersion);
        }

    private:
        struct EntityInfo {
            ComponentMask component_mask {};
            Entity::Version version {};
        };

        /**
         * @brief Maps Entity::Index -> EntityInfo
         *
         * @note In case info is not needed per-entity, a SparseSet may be more
         * suitable.
         */
        std::vector<EntityInfo> m_entity_infos {};

        /**
         * @brief Array of entity indeces already created, but no longer in use -
         * ready to be recycled.
         */
        std::vector<Entity::Index> m_available_entities {};

        /**
         * @brief Associates entity ids with names provided in the create_entity()
         * method. Mainly used for debugging purposes.
         */
        SparseSet<std::string> m_entity_names {};

        /**
         * @brief Arrays of polymorphic component pools.
         * m_component_pools[get_component_index(Component)] == Pool(Component)
         */
        std::vector<std::unique_ptr<ISparseSet>> m_component_pools {};

        /**
         * @brief Array that maps component type indices with component names.
         * m_component_names[get_component_index(Component)] == Name(Component)
         */
        inline static std::vector<std::string> m_component_names {};

        /**
         * @brief Highest recorded entity index, should not exceed MaxEntities.
         */
        Entity::Index m_max_entity_index {};
    };

    template <typename... Components>
    class View {
        /**
         * @brief Gives each component in Components an associated index which we can index.
         */
        using ComponentTypes = TypeList<Components...>;

    public:
        using ForEachFunc = std::function<void(Components&...)>;
        using ForEachFuncWithEntity = std::function<void(Entity, Components&...)>;

        explicit View(ECS* ecs)
            : m_ecs(ecs)
            , m_view_pools { ecs->get_component_pool_ptr<Components>()... } {
            VN_ASSERT(
                ComponentTypes::size == m_view_pools.size(),
                "Component type list and component pool array size must be the same."
            );

            auto smallest_pool = std::min_element(
                m_view_pools.begin(), m_view_pools.end(), [](ISparseSet* lhs, ISparseSet* rhs) {
                    return lhs->size() < rhs->size();
                }
            );

            VN_ASSERT(
                smallest_pool != m_view_pools.end(), "View cannot be initialized empty or invalid."
            );

            m_smallest_pool = *smallest_pool;
        }

        /**
         * @brief Specifies what component should be excluded from this view.
         *
         * Example:
         * ecs.view<A>().without<B, C>();
         * Returns a view that operates on all entites tha have component A, but do NOT have
         * components B or C.
         *
         * @tparam ExcludedComponents The pack of excluded component types.
         * @return View& A reference to this view.
         */
        template <typename... ExcludedComponents>
        [[nodiscard]]
        View& without() const {
            m_excluded_pools = { m_ecs->get_component_pool_ptr<ExcludedComponents>()... };
            return *this;
        }

        /**
         * @brief Calls a passed lambda on all the entities that match the passed parameter pack.
         *
         * Provided lambda should be of the forms:
         * [](Component& c1, Component& c2);
         * OR
         * [](Entity entity, Component& c1, Component& c2);
         *
         * @param func The passed lambda.
         */
        void for_each(ForEachFunc func) {
            for_each_impl(func);
        }

        /**
         * @brief Calls a passed lambda on all the entities that match the passed parameter pack.
         *
         * Provided lambda should be of the forms:
         * [](Component& c1, Component& c2);
         * OR
         * [](Entity entity, Component& c1, Component& c2);
         *
         * @param func The passed lambda.
         */
        void for_each(ForEachFuncWithEntity func) {
            for_each_impl(func);
        }

        /**
         *	@brief Holds { Entity, ...&components } returned by the view; computed on call instead
         *  of while iterating like .for_each() does.
         *
         *	Access components that are part of a pack like such:
         *	auto& [componentA, componentB] = pack.components;
         */
        struct Pack {
            Entity entity {};
            std::tuple<Components&...> components;
        };

        /**
         * @brief Useful when iterating a view through indices.
         *
         * Example:
         * auto packs = ecs.view<A, B>().packed();
         * for (std::size_t i = 0; i < packs.size(); i++) {
         *     auto& [a1, b1] = packed[i].components;
         * }
         *
         * @return std::vector<Pack>
         */
        std::vector<Pack> packed() {
            constexpr auto ComponentIndices = std::make_index_sequence<sizeof...(Components)> {};
            std::vector<Pack> result;

            for (Entity::Index entity_index : m_smallest_pool->get_index_array()) {
                if (all_contain(entity_index) && not_excluded(entity_index)) {
                    result.push_back(
                        { entity_from_index(entity_index),
                          component_tuple(entity_index, ComponentIndices) }
                    );
                }
            }

            return result;
        }

    private:
        /**
         * @brief Returns true iff all the pools in the view contain the given entity index.
         *
         * @param entity_index The entity index.
         * @return true Iff all the pools in the view contain the given entity index.
         * @return false If even one of the pools does not contain the entity index.
         */
        [[nodiscard]]
        bool all_contain(Entity::Index entity_index) const {
            return std::all_of(
                m_view_pools.begin(), m_view_pools.end(), [entity_index](ISparseSet* pool) {
                    return pool->contains_index(entity_index);
                }
            );
        }

        /**
         * @brief Returns true iff the entity index does not exist in at least one excluded pool.
         *d
         * @param entity_index The entity index.
         * @return true Iff the entity index does not exist in at least one excluded pool.
         * @return false If the entity index exists in at least one of the excluded pools.
         */
        [[nodiscard]]
        bool not_excluded(Entity::Index entity_index) const {
            if (m_excluded_pools.empty()) {
                return true;
            }
            return std::none_of(
                m_excluded_pools.begin(), m_excluded_pools.end(), [entity_index](ISparseSet* pool) {
                    return pool->contains_index(entity_index);
                }
            );
        }

        [[nodiscard]]
        Entity entity_from_index(Entity::Index entity_index) const {
            return { m_ecs->m_entity_infos[entity_index].version, entity_index };
        }

        /**
         * @brief Retrieves a given component pool at an Index in m_view_pools.
         *
         */
        template <std::size_t Index>
        auto get_pool() {
            using ComponentType = typename ComponentTypes::template get<Index>;
            return static_cast<SparseSet<ComponentType>*>(m_view_pools[Index]);
        }

        template <std::size_t... Indices>
        auto
        component_tuple(Entity::Index entity_index, std::index_sequence<Indices...> /*unused*/) {
            return std::make_tuple((std::ref(get_pool<Indices>()->get_ref(entity_index)))...);
        }

        /**
         * @brief Iterates over the smallest component pool and runs the provided lambda on all
         * entitites that contain all of the components in the view.
         *
         * @note This is an internal implementation, wrapped by the public querying API.
         *
         * @tparam Func Function template.
         * @param func The passed lambda to be ran on all entities that contain all of the
         * components in the view.
         */
        template <typename Func>
        void for_each_impl(Func&& func) {
            constexpr auto ComponentIndices = std::make_index_sequence<sizeof...(Components)> {};

            // Iterate the smallest component pool and compare against the other pools in the view.
            // NOTE: This index array is a copy, so component deletion during iteration is safe.
            for (Entity::Index component_index : m_smallest_pool->get_index_array()) {
                if (all_contain(component_index) && not_excluded(component_index)) {
                    // This branch is for [](Entity entity, Component& c1, Component& c2, ...);
                    // The whole thing is evaluated at compile time, pruning invalid function call
                    // branches before runtime to prevent invoke errors.
                    if constexpr (std::is_invocable_v<Func, Entity, Components&...>) {
                        Entity entity = entity_from_index(component_index);
                        std::apply(
                            func,
                            std::tuple_cat(
                                std::make_tuple(entity),
                                component_tuple(component_index, ComponentIndices)
                            )
                        );
                    }

                    // This branch is for [](Component& c1, Component& c2, ...);
                    else if constexpr (std::is_invocable_v<Func, Components&...>) {
                        std::apply(func, component_tuple(component_index, ComponentIndices));
                    }

                    else {
                        VN_FATAL(
                            "Bad lambda provided to .for_each(): Parameter pack does not match lambda args."
                        );
                    }
                }
            }
        }

    private:
        ECS* m_ecs {};

        /**
         * @brief Stores component pools associated with the view via their component index from
         * ComponentTypes.
         */
        std::array<ISparseSet*, sizeof...(Components)> m_view_pools;

        std::vector<ISparseSet*> m_excluded_pools {};

        /**
         * @brief Pointer to the smallest component pool that we use as the basis of iteration.
         */
        ISparseSet* m_smallest_pool {};
    };
} // namespace vn