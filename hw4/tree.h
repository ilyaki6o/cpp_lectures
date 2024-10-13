#pragma once

#include <memory>

namespace NBinTree {

template <typename T>
/* 1. Используем наслодование от std::enable_shared_from_this
чтобы избежать повторного удаления this после приведения this к shared_ptr */
class TNode: public std::enable_shared_from_this<TNode<T>> {
public:
    using TNodePtr = std::shared_ptr<TNode<T>>;
    using TNodeConstPtr = std::shared_ptr<const TNode<T>>;
    /* 2. Далее будем использовать weak_ptr как указатель на родительский объект */
    using TWNodePtr = std::weak_ptr<TNode<T>>;
    using TWNodeConstPtr = std::weak_ptr<const TNode<T>>;

    bool HasLeft() const {
        return Left;
    }

    bool HasRight() const {
        return Right;
    }

    bool HasParent() const {
        /* 3. Проверяем, что счетчик ссылок shared_ptr еще не обнулен */
        return !Parent.expired();
    }

    T& GetValue() {
        return Value;
    }

    const T& GetValue() const {
        return Value;
    }

    TNodePtr GetLeft() {
        return Left;
    }

    TNodeConstPtr GetLeft() const {
        return Left;
    }

    TNodePtr GetRight() {
        return Right;
    }

    TNodeConstPtr GetRight() const {
        return Right;
    }

    /* 4. Так как указатели на Parent теперь weak_ptr перед их возвращением
    из функций GetParent их необходимо преобразовать в shared_ptr. */
    TNodePtr GetParent() {
        return this->HasParent() ? Parent.lock() : nullptr;
    }

    TNodeConstPtr GetParent() const {
        return this->HasParent() ? Parent.lock() : nullptr;
    }

    static TNodePtr CreateLeaf(T value) {
        /* 5. Необходимо явно вызывать конструктор класса, так как он приватный. */
        return std::make_shared<TNode>(TNode(value));
    }

    /* 6. Необходимо передавать умные указатели в качестве параметров. */
    static TNodePtr Fork(T value, TNodePtr left, TNodePtr right) {
        /* Аналогично пункту 5. */
        TNodePtr ptr = std::make_shared<TNode>(TNode(value, left, right));
        SetParent(ptr->GetLeft(), ptr);
        SetParent(ptr->GetRight(), ptr);
        return ptr;
    }

    TNodePtr ReplaceLeft(TNodePtr left) {
        /* 7. Так как указателем на Parent является waek_ptr то необходимо
        передавать его в качестве параметра. */
        SetParent(left, this->weak_from_this());
        SetParent(Left, TWNodePtr());
        std::swap(left, Left);
        return left;
    }

    TNodePtr ReplaceRight(TNodePtr right) {
        /* Аналогично пункту 7. */
        SetParent(right, this->weak_from_this());
        SetParent(Right, TWNodePtr());
        std::swap(right, Right);
        return right;
    }

    TNodePtr ReplaceRightWithLeaf(T value) {
        return ReplaceRight(CreateLeaf(value));
    }

    TNodePtr ReplaceLeftWithLeaf(T value) {
        return ReplaceLeft(CreateLeaf(value));
    }

    TNodePtr RemoveLeft() {
        return ReplaceLeft(nullptr);
    }

    TNodePtr RemoveRight() {
        return ReplaceRight(nullptr);
    }

private:
    T Value;
    TNodePtr Left = nullptr;
    TNodePtr Right = nullptr;
    /* 2. Используем weak_ptr чтобы избежать закольцованномти ссылок
    на родительские и дочерние объекты, таким образом они будут удаляться корректно */
    TWNodePtr Parent = TWNodePtr();

    TNode(T value)
        : Value(value)
    {
    }

    /* Аналогично пункту 6. */
    TNode(T value, TNodePtr left, TNodePtr right)
        : Value(value)
        , Left(left)
        , Right(right)
    {
    }

    /* Из пункта 7. следует что нужно исправить указатель на parent */
    static void SetParent(TNodePtr node, TWNodePtr parent) {
        if (node) {
            node->Parent = parent;
        }
    }
};

} // namespace NBinTree
