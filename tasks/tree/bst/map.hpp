#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    Map() : comp_(), root_(nullptr), size_(0) {
    }

    Value& operator[](const Key& key) {
        if (root_ == nullptr) {
            root_ = new Node(key);
            ++size_;
            return root_->value_;
        }

        if (!comp_(key, root_->key_) && !comp_(root_->key_, key)) {
            return root_->value_;
        } else {
            Node* parent = ReturnParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_ != nullptr) {
                    return parent->left_->value_;
                } else {
                    parent->left_ = new Node(key);
                    ++size_;
                    return parent->left_->value_;
                }
            } else {
                if (parent->right_ != nullptr) {
                    return parent->right_->value_;
                } else {
                    parent->right_ = new Node(key);
                    ++size_;
                    return parent->right_->value_;
                }
            }
        }
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(Map& a) {
        static_assert(std::is_same<decltype(this->comp_), decltype(a.comp_)>::value,
                      "The compare function types are different");
        std::swap(root_, a.root_);
        std::swap(size_, a.size_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> result;
        if (root_ == nullptr) {
            return result;
        }
        if (is_increase) {
            IncreaseValues(root_, result);
        } else {
            DecreaseValues(root_, result);
        }
        return result;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        if (root_ == nullptr) {
            root_ = new Node(val);
        } else if (!comp_(root_->key_, val.first) && !comp_(val.first, root_->key_)) {
            root_->value_ = val.second;
        } else {
            Node* parent = ReturnParent(root_, val.first);
            if (comp_(val.first, parent->key_)) {
                if (parent->left_ != nullptr) {
                    parent->left_->value_ = val.second;
                } else {
                    parent->left_ = new Node(val);
                }
            } else {
                if (parent->right_ != nullptr) {
                    parent->right_->value_ = val.second;
                } else {
                    parent->right_ = new Node(val);
                }
            }
        }
        ++size_;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (std::pair<const Key, Value> val : values) {
            Insert(val);
        }
    }

    void Erase(const Key& key) {
        if (root_ == nullptr) {
            throw KeyIsMissingInMap("Value not found");
        } else if (!comp_(key, root_->key_) && !comp_(root_->key_, key)) {
            DeleteRoot();
        } else {
            Node* parent = ReturnParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_ != nullptr) {
                    DeleteNode(parent, parent->left_, false);
                } else {
                    throw KeyIsMissingInMap("Value not found");
                }
            } else {
                if (parent->right_ != nullptr) {
                    DeleteNode(parent, parent->right_, true);
                } else {
                    throw KeyIsMissingInMap("Value not found");
                }
            }
        }
        --size_;
    }

    void Clear() noexcept {
        RecursionClear(root_);
        root_ = nullptr;
        size_ = 0;
    }

    bool Find(const Key& key) const {
        if (root_ == nullptr) {
            return false;
        } else if (!comp_(key, root_->key_) && !comp_(root_->key_, key)) {
            return true;
        } else {
            Node* parent = ReturnParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_ != nullptr) {
                    return true;
                } else {
                    return false;
                }
            } else {
                if (parent->right_ != nullptr) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    ~Map() {
        Clear();
    }

private:
    class Node {
        friend class Map;

    private:
        Key key_;
        Value value_;
        Node* left_;
        Node* right_;

    private:
        Node() : left_(nullptr), right_(nullptr){};

        explicit Node(std::pair<const Key, Value> data) : Node() {
            key_ = data.first;
            value_ = data.second;
        }

        explicit Node(Key key, Value value = 0) : Node() {
            this->value_ = value;
            this->key_ = key;
        }
    };

private:
    void RecursionClear(Node* iter_node) {
        if (iter_node != nullptr) {
            RecursionClear(iter_node->left_);
            RecursionClear(iter_node->right_);
            delete iter_node;
        }
    }

    Node* ReturnParent(Node* start_point, Key key) const {
        Node* parent = start_point;
        Node* current = nullptr;
        if (comp_(key, parent->key_)) {
            current = parent->left_;
        } else {
            current = parent->right_;
        }
        while (current != nullptr) {
            if (!comp_(key, current->key_) && !comp_(current->key_, key)) {
                return parent;
            }
            parent = current;
            if (comp_(key, current->key_)) {
                current = current->left_;
            } else if (comp_(current->key_, key)) {
                current = current->right_;
            }
        }
        return parent;
    }

    void DeleteNode(Node* parent, Node* current, bool is_right) {
        if (current->left_ == nullptr && current->right_ == nullptr) {
            delete current;
            if (is_right) {
                parent->right_ = nullptr;
            } else {
                parent->left_ = nullptr;
            }
        } else if (current->left_ != nullptr && current->right_ != nullptr) {
            IfTwoSons(parent, current, current->left_, is_right);
        } else {
            if (is_right) {
                if (current->left_) {
                    parent->right_ = current->left_;
                } else {
                    parent->right_ = current->right_;
                }
            } else {
                if (current->left_) {
                    parent->left_ = current->left_;
                } else {
                    parent->left_ = current->right_;
                }
            }
            delete current;
        }
    }

    void DeleteRoot() {
        if (root_->left_ == nullptr && root_->right_ == nullptr) {
            delete root_;
            root_ = nullptr;
        } else if (root_->left_ != nullptr && root_->right_ != nullptr) {
            IfTwoSonsRoot(root_->left_);
        } else {
            if (root_->left_ != nullptr) {
                Node* current = root_;
                root_ = root_->left_;
                delete current;
            } else if (root_->right_ != nullptr) {
                Node* current = root_;
                root_ = root_->right_;
                delete current;
            }
        }
    }

    void IfTwoSonsRoot(Node* head) {
        Node* flag_node = head->right_;
        Node* head_parent = head;
        if (head->right_ != nullptr) {
            head = head->right_;
            flag_node = head->right_;
            while (flag_node != nullptr) {
                flag_node = flag_node->right_;
                head = head->right_;
                head_parent = head_parent->right_;
            }
            head_parent->right_ = head->left_;
            head->right_ = root_->right_;
            head->left_ = root_->left_;
            delete root_;
            root_ = head;
        } else {
            head->right_ = root_->right_;
            delete root_;
            root_ = head;
        }
    }

    void IfTwoSons(Node* parent, Node* current, Node* head, bool is_right) {
        Node* flag_node = head->right_;
        Node* head_parent = head;

        if (head->right_ != nullptr) {
            while (flag_node->right_) {
                head_parent = head;
                head = head->right_;
                flag_node = flag_node->right_;
            }

            head_parent->right_ = head->left_;
            head->right_ = current->right_;
            head->left_ = current->left_;
            delete current;

            if (is_right) {
                parent->right_ = head;
            } else {
                parent->left_ = head;
            }
        } else {
            head->right_ = current->right_;
            delete current;

            if (is_right) {
                parent->right_ = head;
            } else {
                parent->left_ = head;
            }
        }
    }

    void IncreaseValues(Node* iter_node, std::vector<std::pair<const Key, Value>>& result_vec) const noexcept {
        if (iter_node->left_ != nullptr) {
            this->IncreaseValues(iter_node->left_, result_vec);
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
            if (iter_node->right_) {
                this->IncreaseValues(iter_node->right_, result_vec);
            }
        } else if (iter_node->right_ != nullptr) {
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
            this->IncreaseValues(iter_node->right_, result_vec);
        } else {
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
        }
    }

    void DecreaseValues(Node* iter_node, std::vector<std::pair<const Key, Value>>& result_vec) const noexcept {
        if (iter_node->right_ != nullptr) {
            this->DecreaseValues(iter_node->right_, result_vec);
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
            if (iter_node->left_ != nullptr) {
                this->DecreaseValues(iter_node->left_, result_vec);
            }
        } else if (iter_node->left_ != nullptr) {
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
            this->DecreaseValues(iter_node->left_, result_vec);
        } else {
            result_vec.push_back(std::make_pair(iter_node->key_, iter_node->value_));
        }
    }

private:
    Compare comp_;
    Node* root_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std