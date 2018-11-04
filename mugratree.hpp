#ifndef MUGRATREE_LIB
#define MUGRATREE_LIB

#include <cassert>
#include <deque>

namespace tree
{

struct NodeBase
{
    using SuccsT = std::deque<NodeBase*>;
    explicit NodeBase(NodeBase* pred) : pred_(pred) {}
    NodeBase* pred_;
    SuccsT succs_;
};

template<typename DataT>
struct Node : public NodeBase
{
    Node(DataT* data, NodeBase* pred) : NodeBase(pred), dataPtr_(data) {}
    DataT* dataPtr_;
};

template<typename DataT, typename NodeBasePtrIteratorT>
struct SuccsIterator;

template<typename DataT>
struct Succs
{
    using SuccsT = NodeBase::SuccsT;
    using SuccsIteratorT = SuccsIterator<DataT, SuccsT::iterator>;
    
    Succs(Succs& succs) : succs_(succs) {}

    SuccsIteratorT begin()
    {
        return SuccsIteratorT(succs_.begin());
    }

    SuccsIteratorT end()
    {
        return SuccsIteratorT(succs_.end());
    }

    private:
        SuccsT& succs_;
};

template<typename DataT>
struct Handle
{
    using NodeT = Node<DataT>;
    
    Handle(NodeT* nodePtr) : nodePtr_(nodePtr) {}

    private:
        NodeT* nodePtr_;
};

template<typename DataT, typename NodeBasePtrIteratorT>
struct SuccsIterator
{

    SuccsIterator(NodeBasePtrIteratorT nodeBasePtrIterator) :
        nodeBasePtrIterator_(nodeBasePtrIterator) {}

    DataT& operator*() const
    {
        return *(static_cast<NodeT*>(*nodeBasePtrIterator_)->dataPtr_);
    }

    DataT* operator->() const
    {
        return static_cast<NodeT*>(*nodeBasePtrIterator_)->dataPtr_;
    }

    SuccsIterator& operator++()
    {
        ++nodeBasePtrIterator_;
        return *this;
    }

    SuccsIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    Handle<DataT> GetHandle()
    {
        return Handle<DataT>(static_cast<NodeT*>(&*nodeBasePtrIterator_));
    }

    friend bool operator==(const SuccsIterator& lhs, const SuccsIterator& rhs)
    {
        return lhs.nodeBasePtrIterator_ == rhs.nodeBasePtrIterator_;
    }

    friend bool operator!=(const SuccsIterator& lhs, const SuccsIterator& rhs)
    {
        return lhs.nodeBasePtrIterator_ != rhs.nodeBasePtrIterator_;
    }

    private:
        using NodeT = Node<DataT>;
        NodeBasePtrIteratorT nodeBasePtrIterator_;
};

NodeBase* IncTreeDFIterator(NodeBase* curNodePtr);

template<typename DataT>
struct TreeDFIterator
{
    using NodeT = Node<DataT>;
    explicit TreeDFIterator(NodeBase* nodePtr) : nodePtr_(nodePtr) {}
    
    DataT& operator*() const
    {
        return *(static_cast<NodeT*>(nodePtr_)->dataPtr_);
    }

    DataT* operator->() const
    {
        return static_cast<NodeT*>(nodePtr_)->dataPtr_;
    }

    TreeDFIterator& operator++()
    {
        nodePtr_ = IncTreeDFIterator(nodePtr_);
        return *this;
    }

    TreeDFIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    Handle<DataT> GetHandle()
    {
        return Handle<DataT>(static_cast<NodeT*>(nodePtr_));
    }

    friend bool operator==(const TreeDFIterator& lhs, const TreeDFIterator& rhs)
    {
        return lhs.nodePtr_ == rhs.nodePtr_;
    }

    friend bool operator!=(const TreeDFIterator& lhs, const TreeDFIterator& rhs)
    {
        return lhs.nodePtr_ != rhs.nodePtr_;
    }

    private:
        NodeBase* nodePtr_;
};

template<typename DataT, typename InnerIteratorT>
struct TreeIterator
{
    using NodeT = Node<DataT>;

    TreeIterator(InnerIteratorT nodeIterator) : nodeIterator_(nodeIterator) {}

    DataT& operator*() const
    {
        return *nodeIterator_->dataPtr_;
    }

    DataT* operator->() const
    {
        return nodeIterator_->dataPtr_;
    }

    TreeIterator& operator++()
    {
        ++nodeIterator_;
        return *this;
    }

    TreeIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    Handle<DataT> GetHandle()
    {
        return Handle<DataT>(&*nodeIterator_);
    }

    friend bool operator==(const TreeIterator& lhs, const TreeIterator& rhs)
    {
        return lhs.nodeIterator_ == rhs.nodeIterator_;
    }

    friend bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs)
    {
        return lhs.nodeIterator_ != rhs.nodeIterator_;
    }

    private:
        InnerIteratorT nodeIterator_;
};

template<typename DataT>
struct Tree
{
    using NodeT = Node<DataT>;
    using NodesT = std::deque<NodeT>;
    using TreeIteratorT = TreeIterator<DataT, typename NodesT::iterator>;
    Tree() : root_(nullptr) {}
    explicit Tree(DataT data)
    {
        container_.push_back(std::move(data));
        nodes_.push_back(NodeT(&*container_.rbegin(), nullptr));
        root_ = &*nodes_.rbegin();
    }

    TreeIteratorT begin()
    {
        return TreeIteratorT(nodes_.begin());
    }

    TreeIteratorT end()
    {
        return TreeIteratorT(nodes_.end());
    }

    //TreeIteratorT GetPred(TreeIteratorT cur)
    //{
    //    return cur.GetPred();
    //}

    private:

        std::deque<DataT> container_;
        NodesT nodes_;
        NodeT* root_;
};

} //tree
#endif //MUGRATREE_LIB
