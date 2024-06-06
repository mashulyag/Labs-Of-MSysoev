#pragma once

#include <initializer_list>
#include <memory>
#include <mutex>
#include <utility>

const int DEFAULT_CAP = 10;

template <typename T, class Allocator = std::allocator<T>>
class Vector {
public:
    Vector();

    Vector(size_t count, const T& value);

    Vector(const Vector& other);

    Vector(Vector&& other) noexcept;

    Vector& operator=(const Vector& other);

    Vector& operator=(Vector&& other);

    Vector(std::initializer_list<T> init);

    T& operator[](size_t pos);

    T& Front() const noexcept;

    T& Back() const noexcept;

    T* Data() const noexcept;

    bool IsEmpty() const noexcept;

    size_t Size() const noexcept;

    size_t Capacity() const noexcept;

    void Reserve(size_t new_cap);

    void Clear() noexcept;

    void Insert(size_t pos, T value);

    void Erase(size_t begin_pos, size_t end_pos);

    void PushBack(T value);

    template <class... Args>
    void EmplaceBack(Args&&... args);

    void PopBack();

    void Resize(size_t count, const T& value);

    ~Vector();

private:
    Allocator aloc_;
    T* arr_;
    size_t sz_;
    size_t cap_;
};

template <class Allocator>
class Vector<void*, Allocator> {
public:
    Vector();

    void* Front() const noexcept;

    void* Back() const noexcept;

    void Reserve(size_t new_cap);

    void PushBack(void* value);

    ~Vector();

private:
    Allocator aloc_;
    void** arr_;
    size_t sz_;
    size_t cap_;
};