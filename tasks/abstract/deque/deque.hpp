#pragma once

#include <cstring>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <utility>

#include "exceptions.hpp"

const int CHUNK_SIZE = 10;
const int DEFAULT_CAPACITY = 1;
const int TWO_CHUNKS = 21;

template <typename T, class Allocator = std::allocator<T>>
class Deque {
public:
    Deque() : left_(nullptr), right_(nullptr), left_sz_(0), right_sz_(0), cap_(0){};

    Deque(const Deque& other) : left_sz_(other.left_sz_), right_sz_(other.right_sz_), cap_(other.cap_) {
        left_ = static_cast<T**>(malloc(sizeof(T*) * cap_));
        right_ = static_cast<T**>(malloc(sizeof(T*) * cap_));
        this->copy_chunks(other);
    };

    Deque(Deque&& other) noexcept : left_sz_(other.left_sz_), right_sz_(other.right_sz_), cap_(other.cap_) {
        left_ = static_cast<T**>(malloc(sizeof(T*) * cap_));
        right_ = static_cast<T**>(malloc(sizeof(T*) * cap_));
        this->move_chunks(std::move(other));
        other.left_sz_ = 0;
        other.right_sz_ = 0;
        other.cap_ = 0;
        other.left_ = nullptr;
        other.right_ = nullptr;
    };

    Deque& operator=(const Deque& other) {
        if (this != &other) {
            this->clear();
            left_sz_ = other.left_sz_;
            right_sz_ = other.right_sz_;
            this->copy_chunks(other);
        }
        return *this;
    };

    Deque& operator=(Deque&& other) {
        if (this != &other) {
            this->clear();
            left_sz_ = other.left_sz_;
            right_sz_ = other.right_sz_;
            this->move_chunks(std::move(other));
            other.left_sz_ = 0;
            other.right_sz_ = 0;
            other.cap_ = 0;
            other.left_ = nullptr;
            other.right_ = nullptr;
        }
        return *this;
    };

    ~Deque() {
        this->clear();
    };

public:
    // NOLINTNEXTLINE
    void push_back(T val) {
        if (cap_ == 0) {
            this->reallocate(DEFAULT_CAPACITY);
            left_[0] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            left_[0][0] = val;
            ++left_sz_;
            return;
        } else if (right_sz_ == CHUNK_SIZE * cap_) {
            this->reallocate(cap_ * 2);
        } else if (right_sz_ + left_sz_ == 0) {
            left_[0] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            left_[0][0] = val;
            ++left_sz_;
            return;
        }
        if (right_sz_ % CHUNK_SIZE == 0) {
            right_[right_sz_ / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
        }
        right_[right_sz_ / CHUNK_SIZE][right_sz_ % CHUNK_SIZE] = val;
        ++right_sz_;
    };

    // NOLINTNEXTLINE
    void push_front(T val) {
        if (cap_ == 0) {
            this->reallocate(DEFAULT_CAPACITY);
        } else if (left_sz_ == CHUNK_SIZE * cap_) {
            this->reallocate(cap_ * 2);
        }
        if (left_sz_ % CHUNK_SIZE == 0) {
            left_[left_sz_ / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
        }
        left_[left_sz_ / CHUNK_SIZE][left_sz_ % CHUNK_SIZE] = val;
        ++left_sz_;
    };

    // NOLINTNEXTLINE
    void pop_back() {
        if (left_sz_ + right_sz_ == 0) {
            throw DequeIsEmptyException("No such element");
        }
        if (right_sz_ <= 1) {
            if (left_sz_ < TWO_CHUNKS) {
                this->centerize();
            } else {
                this->reallocate(cap_);
            }
        }
        if (right_sz_ + left_sz_ == 1) {
            ~T(left_[0][0]);
            free(left_[0]);
            --left_sz_;
            return;
        } else if (right_sz_ == 0) {
            this->centerize();
        }
        ~T(right_[(right_sz_ - 1) / CHUNK_SIZE][(right_sz_ - 1) % CHUNK_SIZE]);
        --right_sz_;
    };

    // NOLINTNEXTLINE
    void pop_front() {
        if (left_sz_ + right_sz_ == 0) {
            throw DequeIsEmptyException("No such element");
        }
        if (left_sz_ == 1) {
            if (right_sz_ < TWO_CHUNKS) {
                this->centerize();
            } else {
                this->reallocate(cap_);
            }
        }
        ~T(left_[(left_sz_ - 1) / CHUNK_SIZE][(left_sz_ - 1) % CHUNK_SIZE]);
        --left_sz_;
    };

    // NOLINTNEXTLINE
    T& back() {
        if (right_sz_ == 0) {
            if (left_sz_ == 0) {
                throw DequeIsEmptyException("No such element");
            } else {
                return left_[0][0];
            }
        }
        return right_[(right_sz_ - 1) / CHUNK_SIZE][(right_sz_ - 1) % CHUNK_SIZE];
    };

    // NOLINTNEXTLINE
    T& front() {
        if (left_sz_ == 0) {
            throw DequeIsEmptyException("No such element");
        }
        return left_[(left_sz_ - 1) / CHUNK_SIZE][(left_sz_ - 1) % CHUNK_SIZE];
    };

    T& operator[](size_t pos) {
        if (pos < left_sz_) {
            return left_[(left_sz_ - pos - 1) / CHUNK_SIZE][(left_sz_ - pos - 1) % CHUNK_SIZE];
        } else {
            size_t remains = pos - left_sz_;
            return right_[remains / CHUNK_SIZE][remains % CHUNK_SIZE];
        }
    };

    // NOLINTNEXTLINE
    void clear() {
        if (cap_ == 0) {
            return;
        }
        size_t left_chunks_count = left_sz_ / CHUNK_SIZE;
        if (left_sz_ % CHUNK_SIZE > 0) {
            ++left_chunks_count;
        }
        size_t right_chunks_count = right_sz_ / CHUNK_SIZE;
        if (right_sz_ % CHUNK_SIZE > 0) {
            ++right_chunks_count;
        }
        for (size_t i = 0; i < left_chunks_count; ++i) {
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                ~T(left_[i][j]);
            }
            free(left_[i]);
        }
        for (size_t i = 0; i < right_chunks_count; ++i) {
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                ~T(right_[i][j]);
            }
            free(right_[i]);
        }
        cap_ = 0;
        left_sz_ = 0;
        right_sz_ = 0;
        free(left_);
        free(right_);
        left_ = nullptr;
        right_ = nullptr;
    };

    // NOLINTNEXTLINE
    size_t size() {
        return left_sz_ + right_sz_;
    };

    // NOLINTNEXTLINE
    size_t capacity() {
        return cap_;
    };

private:
    // NOLINTNEXTLINE
    void copy_chunks(const Deque& other) {
        for (size_t i = 0; i < left_sz_; ++i) {
            left_[i] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            std::memcpy(left_[i], other.left_[i], sizeof(T) * CHUNK_SIZE);
        }
        for (size_t i = 0; i < right_sz_; ++i) {
            right_[i] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            std::memcpy(right_[i], other.right_[i], sizeof(T) * CHUNK_SIZE);
        }
    };

    // NOLINTNEXTLINE
    void move_chunks(Deque&& other) {
        for (size_t i = 0; i < left_sz_; ++i) {
            left_[i] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                left_[i][j] = std::move(other.left_[i][j]);
            }
        }
        for (size_t i = 0; i < right_sz_; ++i) {
            right_[i] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                right_[i][j] = std::move(other.right_[i][j]);
            }
        }
    };

    // NOLINTNEXTLINE
    void reallocate(size_t new_cap) {
        T** new_left = static_cast<T**>(malloc(sizeof(T**) * new_cap));
        T** new_right = static_cast<T**>(malloc(sizeof(T**) * new_cap));
        size_t left_chunks_count = left_sz_ / CHUNK_SIZE;
        if (left_sz_ % CHUNK_SIZE > 0) {
            ++left_chunks_count;
        }
        size_t right_chunks_count = right_sz_ / CHUNK_SIZE;
        if (right_sz_ % CHUNK_SIZE > 0) {
            ++right_chunks_count;
        }
        size_t sz = left_chunks_count + right_chunks_count;
        size_t half = sz / 2;
        if (left_chunks_count > half) {
            for (size_t i = left_chunks_count - half - 1; i < left_chunks_count; ++i) {
                new_left[i + half + 1 - left_chunks_count] = left_[i];
            }
            for (int i = static_cast<int>(left_chunks_count - half - 2); i > -right_chunks_count - 1; --i) {
                if (i >= 0) {
                    new_right[left_chunks_count - half - 2 - i] = left_[i];
                } else {
                    new_right[left_chunks_count - half - 2 - i] = right_[-1 - i];
                }
            }
            left_sz_ -= (left_chunks_count - half) * CHUNK_SIZE;
            right_sz_ += (left_chunks_count - half) * CHUNK_SIZE;
        } else if (right_chunks_count > half) {
            for (size_t i = right_chunks_count - half; i < right_chunks_count; ++i) {
                new_right[i + half - right_chunks_count] = right_[i];
            }
            for (int i = static_cast<int>(right_chunks_count - half - 1); i > -left_chunks_count - 1; --i) {
                if (i >= 0) {
                    new_left[right_chunks_count - half - 1 - i] = right_[i];
                } else {
                    new_left[right_chunks_count - half - 1 - i] = left_[-1 - i];
                }
            }
            left_sz_ += (right_chunks_count - half) * CHUNK_SIZE;
            right_sz_ -= (right_chunks_count - half) * CHUNK_SIZE;
        } else {
            for (size_t i = 0; i < left_chunks_count; ++i) {
                new_left[i] = left_[i];
            }
            for (size_t i = 0; i < right_chunks_count; ++i) {
                new_right[i] = right_[i];
            }
        }
        free(left_);
        free(right_);
        left_ = new_left;
        right_ = new_right;
        cap_ = new_cap;
    };

    // NOLINTNEXTLINE
    void centerize() {
        T** new_left = static_cast<T**>(malloc(sizeof(T**) * cap_));
        T** new_right = static_cast<T**>(malloc(sizeof(T**) * cap_));
        size_t sz = left_sz_ + right_sz_;
        size_t sz_of_right_half = sz / 2;
        size_t sz_of_left_half = sz - sz_of_right_half;
        size_t new_left_sz = 0;
        size_t new_right_sz = 0;
        if (left_sz_ < right_sz_) {
            for (size_t i = sz_of_left_half; i > 0; --i) {
                if (i > left_sz_) {
                    if (new_left_sz % CHUNK_SIZE == 0) {
                        new_left[new_left_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                    }
                    new_left[new_left_sz / CHUNK_SIZE][new_left_sz % CHUNK_SIZE] =
                        right_[(right_sz_ - sz_of_right_half - (sz_of_left_half - i) - 1) / CHUNK_SIZE]
                              [(right_sz_ - sz_of_right_half - (sz_of_left_half - i) - 1) % CHUNK_SIZE];
                    ++new_left_sz;
                } else {
                    if (new_left_sz % CHUNK_SIZE == 0) {
                        new_left[new_left_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                    }
                    new_left[new_left_sz / CHUNK_SIZE][new_left_sz % CHUNK_SIZE] =
                        left_[(i - 1) / CHUNK_SIZE][(i - 1) % CHUNK_SIZE];
                    ++new_left_sz;
                }
            }
            for (size_t i = 0; i < sz_of_right_half; ++i) {
                if (new_right_sz % CHUNK_SIZE == 0) {
                    new_right[new_right_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                }
                new_right[new_right_sz / CHUNK_SIZE][new_right_sz % CHUNK_SIZE] =
                    right_[(right_sz_ - new_left_sz + i) / CHUNK_SIZE][(right_sz_ - new_left_sz + i) % CHUNK_SIZE];
                ++new_right_sz;
            }
        } else {
            for (size_t i = left_sz_ - sz_of_left_half; i < left_sz_; ++i) {
                if (new_left_sz % CHUNK_SIZE == 0) {
                    new_left[new_left_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                }
                new_left[new_left_sz / CHUNK_SIZE][new_left_sz % CHUNK_SIZE] = left_[i / CHUNK_SIZE][i % CHUNK_SIZE];
                ++new_left_sz;
            }
            for (size_t i = sz_of_right_half; i > 0; --i) {
                if (i > right_sz_) {
                    if (new_right_sz % CHUNK_SIZE == 0) {
                        new_right[new_right_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                    }
                    new_right[new_right_sz / CHUNK_SIZE][new_right_sz % CHUNK_SIZE] =
                        left_[(left_sz_ - sz_of_left_half - 1) / CHUNK_SIZE]
                             [(left_sz_ - sz_of_left_half - 1) % CHUNK_SIZE];
                    ++new_right_sz;
                } else {
                    if (new_right_sz % CHUNK_SIZE == 0) {
                        new_right[new_right_sz / CHUNK_SIZE] = static_cast<T*>(malloc(sizeof(T) * CHUNK_SIZE));
                    }
                    new_right[new_right_sz / CHUNK_SIZE][new_right_sz % CHUNK_SIZE] =
                        right_[(i - 1) / CHUNK_SIZE][(i - 1) % CHUNK_SIZE];
                    ++new_right_sz;
                }
            }
        }
        free(left_);
        free(right_);
        left_ = new_left;
        right_ = new_right;
        left_sz_ = sz_of_left_half;
        right_sz_ = sz_of_right_half;
    };

private:
    T** left_;
    T** right_;
    size_t left_sz_;
    size_t right_sz_;
    size_t cap_;
};