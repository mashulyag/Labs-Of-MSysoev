#include "vector.hpp"

template <typename T, class Allocator>
Vector<T, Allocator>::Vector() {
    arr_ = nullptr;
    sz_ = 0;
    cap_ = 0;
}

template <typename T, class Allocator>
Vector<T, Allocator>::Vector(size_t count, const T& value) : Vector() {
    cap_ = DEFAULT_CAP;
    arr_ = aloc_.allocate(cap_);
    for (size_t i = 0; i < count; ++i) {
        this->PushBack(value);
    }
}

template <typename T, class Allocator>
Vector<T, Allocator>::Vector(const Vector& other) : sz_(other.sz_), cap_(other.cap_) {
    arr_ = aloc_.allocate(other.cap_);
    std::uninitialized_copy(other.arr_, other.arr_ + other.sz_, arr_);
}

template <typename T, class Allocator>
Vector<T, Allocator>::Vector(Vector&& other) noexcept : arr_(other.arr_), sz_(other.sz_), cap_(other.cap_) {
    other.sz_ = 0;
    other.cap_ = 0;
    other.arr_ = nullptr;
}

template <typename T, class Allocator>
Vector<T, Allocator>::Vector(std::initializer_list<T> init) : arr_(nullptr), sz_(0), cap_(DEFAULT_CAP) {
    arr_ = aloc_.allocate(cap_);
    for (const T& val : init) {
        this->PushBack(val);
    }
}

template <typename T, class Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other) {
    if (this != &other) {
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        cap_ = other.cap_;
        arr_ = aloc_.allocate(cap_);
        sz_ = other.sz_;
        std::uninitialized_copy(other.arr_, other.arr_ + other.sz_, arr_);
    }
    return *this;
}

template <typename T, class Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& other) {
    if (this != &other) {
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        arr_ = other.arr_;
        sz_ = other.sz_;
        cap_ = other.cap_;
        other.cap_ = 0;
        other.sz_ = 0;
        other.arr_ = nullptr;
    }
    return *this;
}

template <typename T, class Allocator>
T& Vector<T, Allocator>::operator[](size_t pos) {
    return *(arr_ + pos);
}

template <typename T, class Allocator>
T& Vector<T, Allocator>::Front() const noexcept {
    return *arr_;
}

template <typename T, class Allocator>
bool Vector<T, Allocator>::IsEmpty() const noexcept {
    return sz_ == 0;
}

template <typename T, class Allocator>
T& Vector<T, Allocator>::Back() const noexcept {
    return *(arr_ + sz_ - 1);
}

template <typename T, class Allocator>
T* Vector<T, Allocator>::Data() const noexcept {
    return arr_;
}

template <typename T, class Allocator>
size_t Vector<T, Allocator>::Size() const noexcept {
    return sz_;
}

template <typename T, class Allocator>
size_t Vector<T, Allocator>::Capacity() const noexcept {
    return cap_;
}

template <typename T, class Allocator>
void Vector<T, Allocator>::Reserve(size_t new_cap) {
    if (new_cap > cap_) {
        T* new_arr = aloc_.allocate(new_cap);
        size_t old_size = sz_;
        for (size_t i = 0; i < sz_; ++i) {
            new (new_arr + i) T(std::move(arr_[i]));
        }
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        sz_ = old_size;
        arr_ = new_arr;
        cap_ = new_cap;
    }
}

template <typename T, class Allocator>
void Vector<T, Allocator>::Clear() noexcept {
    for (size_t i = 0; i < sz_; ++i) {
        (arr_ + i)->~T();
    }
    sz_ = 0;
}

template <typename T, class Allocator>
void Vector<T, Allocator>::Insert(size_t pos, T value) {
    for (size_t i = pos; i < sz_; ++i) {
        std::swap(value, arr_[i]);
    }
    this->PushBack(value);
}

template <typename T, class Allocator>
void Vector<T, Allocator>::Erase(size_t begin_pos, size_t end_pos) {
    if (begin_pos < end_pos && begin_pos < sz_) {
        T* new_arr = aloc_.allocate(cap_);
        size_t old_size = sz_;
        size_t j = 0;
        for (size_t i = 0; i < begin_pos; ++i) {
            new (new_arr + i) T(std::move(arr_[i]));
            ++j;
        }
        for (size_t i = end_pos; i < sz_; ++i) {
            new (new_arr + j) T(std::move(arr_[i]));
            ++j;
        }
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        arr_ = new_arr;
        sz_ = old_size - end_pos + begin_pos;
    }
}

template <typename T, class Allocator>
void Vector<T, Allocator>::PushBack(T value) {
    this->EmplaceBack(std::move(value));
}

template <typename T, class Allocator>
template <class... Args>
void Vector<T, Allocator>::EmplaceBack(Args&&... args) {
    if (cap_ == 0) {
        this->Reserve(DEFAULT_CAP);
    } else if (cap_ == sz_) {
        this->Reserve(cap_ * 2);
    }
    new (arr_ + sz_) T(std::forward<Args>(args)...);
    ++sz_;
}

template <typename T, class Allocator>
void Vector<T, Allocator>::PopBack() {
    (arr_ + sz_ - 1)->~T();
    --sz_;
}

template <typename T, class Allocator>
void Vector<T, Allocator>::Resize(size_t count, const T& value) {
    if (count > sz_) {
        for (size_t i = sz_; i < count; ++i) {
            this->PushBack(value);
        }
    } else {
        this->Erase(count, sz_);
    }
}

template <typename T, class Allocator>
Vector<T, Allocator>::~Vector() {
    this->Clear();
    aloc_.deallocate(arr_, cap_);
    cap_ = 0;
}

template <class Allocator>
Vector<void*, Allocator>::Vector() : arr_(nullptr), sz_(0), cap_(0) {
    arr_ = nullptr;
    sz_ = 0;
    cap_ = 0;
}

template <class Allocator>
void* Vector<void*, Allocator>::Front() const noexcept {
    return *arr_;
}

template <class Allocator>
void* Vector<void*, Allocator>::Back() const noexcept {
    return *(arr_ + sz_ - 1);
}

template <class Allocator>
void Vector<void*, Allocator>::Reserve(size_t new_cap) {
    if (new_cap > cap_) {
        void** new_arr = static_cast<void**>(aloc_.allocate(new_cap));
        for (size_t i = 0; i < sz_; ++i) {
            new_arr[i] = arr_[i];
        }
        aloc_.deallocate(arr_, cap_);
        arr_ = new_arr;
        cap_ = new_cap;
    }
}

template <class Allocator>
void Vector<void*, Allocator>::PushBack(void* value) {
    if (cap_ == 0) {
        this->Reserve(DEFAULT_CAP);
    } else if (cap_ == sz_) {
        this->Reserve(cap_ * 2);
    }
    arr_[sz_] = value;
    ++sz_;
}

template <class Allocator>
Vector<void*, Allocator>::~Vector() {
    for (size_t i = 0; i < sz_; ++i) {
        free(arr_[i]);
    }
    aloc_.deallocate(arr_, cap_);
}