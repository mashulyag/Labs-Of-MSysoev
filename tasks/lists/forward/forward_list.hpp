#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    class Node {
        friend class ForwardListIterator;
        friend class ForwardList;

    private:
        T data_;
        Node* next_;

        Node() : next_(nullptr) {
        }

        explicit Node(T data) : Node() {
            data_ = data;
        }

        Node(T data, Node* next) : Node(data) {
            next_ = next;
        }
    };

public:
    class ForwardListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using IteratorCategory = std::forward_iterator_tag;

        inline bool operator==(const ForwardListIterator& other) const {
            return current_ == other.current_;
        };

        inline bool operator!=(const ForwardListIterator& other) const {
            return current_ != other.current_;
        };

        inline reference_type operator*() const {
            return current_->data_;
        };

        ForwardListIterator& operator++() {
            current_ = current_->next_;
            return *this;
        };

        ForwardListIterator operator++(int) {
            ForwardListIterator temp(current_);
            current_ = current_->next_;
            return temp;
        };

        pointer_type operator->() const {
            if (current_ == nullptr) {
                throw std::runtime_error("Cannot dereference null iterator");
            }
            return &(current_->data_);
        };

    private:
        friend class ForwardList<T>;
        explicit ForwardListIterator(Node* node_ptr) : current_(node_ptr) {
        }

    private:
        Node* current_;
    };

public:
    ForwardList() : head_(nullptr), size_(0) {
    }

    explicit ForwardList(size_t sz, const T& value = T()) : ForwardList() {
        if (sz != 0) {
            head_ = new Node(value);
            ++size_;
            Node* iter = head_;
            for (size_t i = 1; i < sz; ++i) {
                iter->next_ = new Node(value);
                iter = iter->next_;
                ++size_;
            }
        }
    }

    ForwardList(const std::initializer_list<T>& values) : ForwardList() {
        Node* iter = nullptr;
        for (const T& value : values) {
            if (size_ == 0) {
                head_ = new Node(value);
                iter = head_;
            } else {
                iter->next_ = new Node(value);
                iter = iter->next_;
            }
            ++size_;
        }
    }

    ForwardList(const ForwardList& other) : ForwardList() {
        if (other.size_ > 0) {
            head_ = new Node(other.head_->data_);
            Node* self_iter = head_;
            Node* other_iter = other.head_->next_;
            ++size_;
            while (other_iter != nullptr) {
                self_iter->next_ = new Node(other_iter->data_);
                self_iter = self_iter->next_;
                other_iter = other_iter->next_;
                ++size_;
            }
        }
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            Clear();
            if (other.size_ > 0) {
                head_ = new Node(other.head_->data_);
                Node* self_iter = head_;
                Node* other_iter = other.head_->next_;
                ++size_;
                while (other_iter != nullptr) {
                    self_iter->next_ = new Node(other_iter->data_);
                    self_iter = self_iter->next_;
                    other_iter = other_iter->next_;
                    ++size_;
                }
            }
        }
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(head_);
    }

    ForwardListIterator End() const noexcept {
        return ForwardListIterator(nullptr);
    }

    inline T& Front() const {
        return head_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(ForwardList& a) {
        std::swap(head_, a.head_);
        std::swap(size_, a.size_);
    }

    void EraseAfter(ForwardListIterator pos) {
        if (size_ > 1) {
            Node* iter = head_;
            while (iter != pos.current_) {
                iter = iter->next_;
            }
            Node* current_node = iter->next_;
            iter->next_ = current_node->next_;
            delete current_node;
            --size_;
        } else {
            throw ListIsEmptyException("Value doesn`t exist");
        }
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        Node* iter = head_;
        while (iter != pos.current_) {
            iter = iter->next_;
        }
        iter->next_ = new Node(value, iter->next_);

        ++size_;
    }

    ForwardListIterator Find(const T& value) const {
        Node* iter = head_;
        while (iter->data_ != value) {
            iter = iter->next_;
        }
        return ForwardListIterator(iter);
    }

    void Clear() noexcept {
        if (size_ != 0) {
            Node* current_node = head_->next_;
            Node* next_node = current_node;
            delete head_;
            while (current_node != nullptr) {
                current_node = current_node->next_;
                delete next_node;
                next_node = current_node;
            }
            head_ = nullptr;
            size_ = 0;
        }
    }

    void PushFront(const T& value) {
        head_ = new Node(value, head_);
        ++size_;
    }

    void PopFront() {
        if (size_ > 1) {
            Node* current_node = head_;
            head_ = head_->next_;
            delete current_node;
            --size_;
        } else if (size_ == 1) {
            delete head_;
            head_ = nullptr;
            --size_;
        } else {
            throw ListIsEmptyException("Value doesn`t exist");
        }
    }

    ~ForwardList() {
        Clear();
    }

private:
    Node* head_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename T>
void Swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std