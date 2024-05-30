#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class Deque {
private:
    class Node {
        friend class DequeIterator;
        friend class Deque;

    private:
        T data_;
        Node* next_ = nullptr;
        Node* prev_ = nullptr;
    };

public:
    class DequeIterator {
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
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const DequeIterator& other) const {
            return this->current_ == other.current_;
        };
        inline bool operator!=(const DequeIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        DequeIterator& operator++() {
            this->current_ = this->current_->next_;
            return *this;
        };

        DequeIterator operator++(int) {
            DequeIterator new_iter(this->current_);
            this->current_ = this->current_->next_;
            return new_iter;
        };

        DequeIterator& operator--() {
            this->current_ = this->current_->prev_;
            return *this;
        };

        DequeIterator operator--(int) {
            DequeIterator new_iter(this->current_);
            this->current_ = this->current_->prev_;
            return new_iter;
        };

        inline pointer_type operator->() const {
            if (current_ != nullptr) {
                return &(current_->data_);
            } else {
                return nullptr;
            }
        };

    private:
        friend class Deque<T>;
        explicit DequeIterator(Node* node_ptr) : current_(node_ptr) {
        }

    private:
        Node* current_;
    };

public:
    Deque() : head_(nullptr), tail_(nullptr), size_(0) {
    }

    explicit Deque(size_t sz) : Deque() {
        if (sz != 0) {
            Node* first_node = new Node();
            Node* last_node = new Node();
            head_ = first_node;
            tail_ = last_node;
            head_->next_ = tail_;
            tail_->prev_ = head_;
            ++size_;
            for (size_t i = 1; i < sz; ++i) {
                Node* new_node = new Node();
                tail_->next_ = new_node;
                new_node->prev_ = tail_;
                tail_ = new_node;
                ++size_;
            }
        }
    }

    Deque(const std::initializer_list<T>& values) : Deque() {
        for (const auto& value : values) {
            PushBack(value);
        }
    }

    Deque(const Deque& other) : Deque() {
        Node* iter = other.head_;
        while (iter != other.tail_) {
            this->PushBack(iter->data_);
            iter = iter->next_;
        }
    }

    Deque& operator=(const Deque& other) {
        if (this == &other) {
            return *this;
        }
        this->Clear();
        Node* iter = other.head_;
        while (iter != other.tail_) {
            PushBack(iter->data_);
            iter = iter->next_;
        }
        return *this;
    }

    DequeIterator Begin() const noexcept {
        return DequeIterator(head_);
    }

    DequeIterator End() const noexcept {
        return DequeIterator(tail_);
    }

    inline T& Front() const {
        return head_->data_;
    }

    inline T& Back() const {
        return tail_->prev_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(Deque& other) {
        std::swap(this->head_, other.head_);
        std::swap(this->tail_, other.tail_);
        std::swap(this->size_, other.size_);
    }

    void PushBack(const T& value) {
        if (head_ == nullptr) {
            head_ = new Node(value);
            tail_ = new Node();
            head_->next_ = tail_;
            tail_->prev_ = head_;
            ++size_;
        } else {
            Node* new_node = new Node();
            tail_->data_ = value;
            tail_->next_ = new_node;
            new_node->prev_ = tail_;
            tail_ = new_node;
            ++size_;
        }
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = head_;
        head_->prev_ = new_node;
        head_ = new_node;
        ++size_;
    }

    T PopBack() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("Deque is empty");
        }
        T result = this->Back();
        if (size_ == 1) {
            delete head_;
            delete tail_;
            head_ = nullptr;
            tail_ = nullptr;
        } else {
            tail_->prev_ = tail_->prev_->prev_;
            delete tail_->prev_->next_;
            tail_->prev_->next_ = tail_;
        }
        --size_;
        return result;
    }

    T PopFront() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("Deque is empty");
        }
        T result = this->Back();
        if (size_ == 1) {
            delete head_;
            delete tail_;
            head_ = nullptr;
            tail_ = nullptr;
        } else {
            head_ = head_->next_;
            delete head_->prev_;
            head_->prev_ = nullptr;
        }
        --size_;
        return result;
    }

    ~Deque() {
        while (head_ != tail_) {
            head_ = head_->next_;
            delete head_->prev_;
        }
        if (tail_ != nullptr) {
            delete tail_;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_;
    Node* tail_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(Deque<T>& a, Deque<T>& b) {
    a.Swap(b);
}
}  // namespace std