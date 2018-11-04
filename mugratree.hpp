#ifndef MUGRATREE_LIB
#define MUGRATREE_LIB

#include <cassert>
#include <deque>

namespace mugratree
{

struct NodeBase
{
    NodeBase* pred_;
    std::deque<NodeBase*> succs_;
};

template<typename DataT>
struct Node : public NodeBase
{
    DataT* data_;
};

template<typename DataT>
struct Iterator
{
    using NodeT = Node<DataT>;
    
    Iterator(NodeBase* nodePtr) : nodePtr_(nodePtr) {}

    DataT& operator*() const
    {
        return *(static_cast<NodeT*>(nodePtr_)->data_);
    }

    DataT* operator->() const
    {
        return static_cast<NodeT*>(nodePtr_)->data_;
    }

    private:
        NodeBase* nodePtr_;
};

template<typename DataT>
class Tree
{
    public:

    private:
        using NodeT = Node<DataT>;

        std::deque<DataT> container_;
        std::deque<NodeT> nodes_;
        NodeT* root_;
};

} //mugratree
#endif //MUGRATREE_LIB
