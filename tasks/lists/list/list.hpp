#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <class T>
class List {
private:
    size_t size_;

    struct BaseNode {
        BaseNode* nextNode_;
        BaseNode* prevNode_;

        BaseNode(BaseNode* next, BaseNode* prev) : nextNode_(next), prevNode_(prev) {
        }
    };

    struct Node : public BaseNode {
        T value;

        Node(BaseNode* next, BaseNode* prev, const T& value) : BaseNode(next, prev), value(value) {
        }
    };

    BaseNode sentinel_node_;

    struct ListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = T&;
        // NOLINTNEXTLINE
        using pointer_type = T*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        BaseNode* node_ptr_;
        friend class List<T>;

        explicit ListIterator(BaseNode* ptr) : node_ptr_(ptr) {
        }

    public:
        T& operator*() {
            return static_cast<Node*>(node_ptr_)->value;
        }

        ListIterator& operator++() {
            node_ptr_ = node_ptr_->nextNode_;
            return *this;
        }

        ListIterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        ListIterator& operator--() {
            node_ptr_ = node_ptr_->prevNode_;
            return *this;
        }

        ListIterator operator--(int) {
            auto copy = *this;
            --(*this);
            return copy;
        }

        inline bool operator==(const ListIterator& other) const {
            return (node_ptr_ == other.node_ptr_);
        }

        inline bool operator!=(const ListIterator& other) const {
            return !(other == *this);
        }

        inline T* operator->() const {
            return &(static_cast<Node*>(node_ptr_)->value);
        };
    };

public:
    using Iterator = ListIterator;

    List() : size_(0), sentinel_node_(&sentinel_node_, &sentinel_node_) {
    }

    explicit List(size_t count) : List() {
        while (size_ < count) {
            Insert(End(), T());
        }
    }

    List(const List& other) : List() {
        Node* node = static_cast<Node*>(other.sentinel_node_.nextNode_);
        while (size_ < other.Size()) {
            Insert(End(), node->value);
            node = static_cast<Node*>(node->nextNode_);
        }
    }

    List(const std::initializer_list<T>& other) : List() {
        auto it = other.begin();
        while (size_ < other.size()) {
            Insert(End(), *(it++));
        }
    }

    List& operator=(const List& other) {
        if (&other != this) {
            List<T> copy = other;
            this->Swap(copy);
        }
        return *this;
    }

    void Swap(List& other) {
        if (other.size_ != 0) {
            other.sentinel_node_.nextNode_->prevNode_ = &sentinel_node_;
            other.sentinel_node_.prevNode_->nextNode_ = &sentinel_node_;
            if (size_ != 0) {
                sentinel_node_.nextNode_->prevNode_ = &(other.sentinel_node_);
                sentinel_node_.prevNode_->nextNode_ = &(other.sentinel_node_);
                std::swap(sentinel_node_.nextNode_, other.sentinel_node_.nextNode_);
                std::swap(sentinel_node_.prevNode_, other.sentinel_node_.prevNode_);
            } else {
                sentinel_node_.nextNode_ = other.sentinel_node_.nextNode_;
                sentinel_node_.prevNode_ = other.sentinel_node_.prevNode_;
                other.sentinel_node_.nextNode_ = &other.sentinel_node_;
                other.sentinel_node_.prevNode_ = &other.sentinel_node_;
            }
        } else {
            if (size_ != 0) {
                sentinel_node_.nextNode_->prevNode_ = &other.sentinel_node_;
                sentinel_node_.prevNode_->nextNode_ = &other.sentinel_node_;
                other.sentinel_node_.nextNode_ = sentinel_node_.nextNode_;
                other.sentinel_node_.prevNode_ = sentinel_node_.prevNode_;
                sentinel_node_.nextNode_ = &sentinel_node_;
                sentinel_node_.prevNode_ = &sentinel_node_;
            }
        }
        std::swap(size_, other.size_);
    }

    void Clear() {
        Node* node = static_cast<Node*>(sentinel_node_.nextNode_);
        Node* next_node = nullptr;
        while (size_ > 0) {
            --size_;
            next_node = static_cast<Node*>(node->nextNode_);
            node->~Node();
            operator delete(node);
            node = next_node;
        }
        sentinel_node_.nextNode_ = &sentinel_node_;
        sentinel_node_.prevNode_ = &sentinel_node_;
    }

    Iterator Begin() {
        return Iterator(sentinel_node_.nextNode_);
    }

    Iterator End() {
        return Iterator(&sentinel_node_);
    }

    void Erase(Iterator it) {
        if (size_ == 0) {
            throw ListIsEmptyException("...");
        }
        it.node_ptr_->nextNode_->prevNode_ = it.node_ptr_->prevNode_;
        it.node_ptr_->prevNode_->nextNode_ = it.node_ptr_->nextNode_;
        static_cast<Node*>(it.node_ptr_)->~Node();
        operator delete(static_cast<Node*>(it.node_ptr_));
        --size_;
    }

    void PushBack(const T& value) {
        Node* new_node = new Node(&sentinel_node_, sentinel_node_.prevNode_, value);
        new_node->prevNode_->nextNode_ = new_node;
        sentinel_node_.prevNode_ = new_node;
        ++size_;
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(sentinel_node_.nextNode_, &sentinel_node_, value);
        new_node->nextNode_->prevNode_ = new_node;
        sentinel_node_.nextNode_ = new_node;
        ++size_;
    }

    void PopFront() {
        if (IsEmpty()) {
            throw ListIsEmptyException("...");
        }
        Node* node_to_delete = static_cast<Node*>(sentinel_node_.nextNode_);
        sentinel_node_.nextNode_ = node_to_delete->nextNode_;
        node_to_delete->nextNode_->prevNode_ = &sentinel_node_;
        delete node_to_delete;
        --size_;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw ListIsEmptyException("...");
        }
        Node* node_to_delete = static_cast<Node*>(sentinel_node_.prevNode_);
        sentinel_node_.prevNode_ = node_to_delete->prevNode_;
        node_to_delete->prevNode_->nextNode_ = &sentinel_node_;
        delete node_to_delete;
        --size_;
    }

    inline T& Back() {
        return static_cast<Node*>(sentinel_node_.prevNode_)->value;
    }

    inline T& Front() {
        return static_cast<Node*>(sentinel_node_.nextNode_)->value;
    }

    void Insert(Iterator it, const T& value) {
        Node* node = static_cast<Node*>(operator new(sizeof(Node)));
        new (node) Node(it.node_ptr_, it.node_ptr_->prevNode_, value);
        it.node_ptr_->prevNode_->nextNode_ = node;
        it.node_ptr_->prevNode_ = node;
        ++size_;
    }

    Iterator Find(const T& value) const {
        Node* current_node = static_cast<Node*>(sentinel_node_.nextNode_);
        while (current_node != &sentinel_node_) {
            if (current_node->value == value) {
                break;
            }
            current_node = static_cast<Node*>(current_node->nextNode_);
        }
        return Iterator(current_node);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    ~List() {
        Clear();
    }
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std