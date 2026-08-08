#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#include "vinter/logger.hpp"

namespace vn {
    // Base class of SparseSet that allows runtime polymprphism.
    class ISparseSet {
    public:
        using SparseIndex = std::size_t;
        using DenseIndex = std::size_t;

        virtual ~ISparseSet() = default;

        virtual void unset(SparseIndex index) = 0;

        virtual void clear() = 0;

        [[nodiscard]]
        virtual std::size_t size() const noexcept = 0;

        [[nodiscard]]
        virtual bool contains_index(SparseIndex index) const = 0;

        [[nodiscard]]
        virtual std::vector<SparseIndex> get_index_array() const noexcept = 0;
    };

    template <typename T>
    class SparseSet : ISparseSet {
    public:
        explicit SparseSet(std::size_t initial_size = 1000) {
            m_sparse.reserve(initial_size);
            m_dense.reserve(initial_size);
            m_dense_to_sparse_index.reserve(initial_size);
        }

        // Upserts into the dense array such that:
        // m_dense[m_sparse[sparse_index]] == T
        template <typename U>
        T* set(SparseIndex sparse_index, U&& obj) {
            const DenseIndex dense_index = get_dense_index(sparse_index);

            // Overwrite if data exists at entity index.
            if (dense_index != Tombstone) {
                m_dense[dense_index] = std::forward<U>(obj);
                return &m_dense[dense_index];
            }

            // Map sparse index to dense index.
            set_sparse_index(sparse_index, m_dense.size());

            // Push data to the back of the dense array.
            m_dense.push_back(std::forward<U>(obj));
            m_dense_to_sparse_index.push_back(sparse_index);

            return &m_dense.back();
        }

        // Returns a pointer to T if data exists in the dense array, or nullptr if not.
        [[nodiscard]]
        T* get(SparseIndex sparse_index) const {
            const DenseIndex dense_index = get_dense_index(sparse_index);
            if (dense_index == Tombstone) {
                return nullptr;
            }
            return &m_dense[dense_index];
        }

        // Returns a reference to T, always assuming the data exists in the dense array.
        [[nodiscard]]
        T& get_ref(SparseIndex sparse_index) const {
            const DenseIndex dense_index = get_dense_index(sparse_index);
            VN_ASSERT(
                dense_index != Tombstone,
                "Tried to get reference from invalid sparse index {}",
                sparse_index
            );
            return m_dense[dense_index];
        }

        // Returns a const reference to T, always assuming the data exists in the dense array.
        [[nodiscard]]
        const T& get_const_ref(SparseIndex sparse_index) const {
            const DenseIndex dense_index = get_dense_index(sparse_index);
            VN_ASSERT(
                dense_index != Tombstone,
                "Tried to get reference from invalid sparse index {}",
                sparse_index
            );
            return m_dense[dense_index];
        }

        // Removes data from the dense array, and sets the sparse index to a tombstone (null) value.
        void unset(SparseIndex sparse_index) override {
            const std::size_t deleted_dense_index = get_dense_index(sparse_index);

            if (m_dense.empty() || deleted_dense_index == Tombstone) {
                return;
            }

            // Swap the back element's index in the dense array to the deleted element's index, and
            // set the deleted element's index to a tombstone (null).
            set_sparse_index(m_dense_to_sparse_index.back(), deleted_dense_index);
            set_sparse_index(sparse_index, Tombstone);

            // Swap deleted index with back element, then pop off the back of the dense list.
            std::swap(m_dense.back(), m_dense[deleted_dense_index]);
            std::swap(m_dense_to_sparse_index.back(), m_dense_to_sparse_index[deleted_dense_index]);

            m_dense.pop_back();
            m_dense_to_sparse_index.pop_back();
        }

        // Returns the size of the dense array.
        [[nodiscard]]
        std::size_t size() const noexcept override {
            return m_dense.size();
        }

        // Returns a copy of all sparse indices that currently have data in the dense list so we
        // can safely delete from the vector while iterating.
        [[nodiscard]]
        std::vector<SparseIndex> get_index_array() const noexcept override {
            return m_dense_to_sparse_index;
        }

        // Check if the sparse index currently has associated data in the dense array.
        [[nodiscard]]
        bool contains_index(SparseIndex sparse_index) const override {
            return get_dense_index(sparse_index) != Tombstone;
        }

        // Erases all the elements.
        void clear() override {
            m_dense.clear();
            m_sparse.clear();
            m_dense_to_sparse_index.clear();
        }

        // Returns true if the dense array is empty.
        [[nodiscard]]
        bool is_empty() const {
            return m_dense.empty();
        }

        // Returns a read-only dense array.
        [[nodiscard]]
        const std::vector<T>& data() const noexcept {
            return m_dense;
        }

    private:
        static constexpr std::size_t Tombstone = std::numeric_limits<std::size_t>::max();

        // Stores index into the dense array, where: (m_dense[m_sparse[sparse_index]] == T)
        std::vector<DenseIndex> m_sparse;

        // Holds (component) data in a contiguous (dense) manner.
        std::vector<T> m_dense;

        // 1:1 vector where dense index == sparse index.
        // Allows querying sparse indices for dense elements.
        std::vector<SparseIndex> m_dense_to_sparse_index;

        // Inserts an index into the sparse list that maps to the dense list, associating
        // m_dense[m_sparse[sparse_index]] == m_dense[dense_index].
        void set_sparse_index(SparseIndex sparse_index, DenseIndex dense_index) {
            if (sparse_index >= m_sparse.size()) {
                m_sparse.resize(sparse_index + 1, Tombstone);
            }
            m_sparse[sparse_index] = dense_index;
        }

        // Returns the dense index for a given sparse index, or a tombstone (null) value if it's
        // non-existent.
        [[nodiscard]]
        DenseIndex get_dense_index(SparseIndex sparse_index) const {
            if (sparse_index >= m_sparse.size()) {
                return Tombstone;
            }
            return m_sparse[sparse_index];
        }
    };
} // namespace vn