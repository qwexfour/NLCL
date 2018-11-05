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
struct TreeDFIterator;

template<typename DataT>
struct Succs
{
    using SuccsT = NodeBase::SuccsT;
    using SuccsIteratorT = SuccsIterator<DataT, SuccsT::iterator>;
    
    Succs(SuccsT& succs) : succs_(succs) {}

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
struct Tree;

template<typename DataT>
struct Handle
{
    using NodeT = Node<DataT>;
    using TreeDFIteratorT = TreeDFIterator<DataT>;
    
    Handle(NodeT* nodePtr) : nodePtr_(nodePtr) {}

    TreeDFIteratorT GetPred()
    {
        return TreeDFIteatorT(nodePtr_->pred_);
    }

    Succs<DataT> GetSuccs()
    {
        return Succs<DataT>(nodePtr_->succs_);
    }

    TreeDFIteratorT AddSucc(NodeT* newNode)
    {
        newNode->pred_ = nodePtr_;
        nodePtr_->succs_.push_back(newNode);
        return TreeDFIteratorT(newNode);
    }

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
        return Handle<DataT>(static_cast<NodeT*>(*nodeBasePtrIterator_));
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
    using TreeDFIteratorT = TreeDFIterator<DataT>;
    
    Tree() : root_(nullptr) {}
    
    explicit Tree(DataT data)
    {
        root_ = AddNode(std::move(data), nullptr);
    }

    TreeDFIteratorT begin()
    {
        return TreeDFIteratorT(root_);
    }

    TreeDFIteratorT end()
    {
        return TreeDFIteratorT(nullptr);
    }

    TreeDFIteratorT GetRoot()
    {
        return TreeDFIteratorT(root_);
    }

    template<typename IteratorT>
    TreeDFIteratorT GetPred(IteratorT cur)
    {
        return cur.GetHandle().GetPred();
    }

    template<typename IteratorT>
    TreeDFIteratorT AddSucc(DataT data, IteratorT cur)
    {
        NodeT* newNode = AddNode(std::move(data), nullptr);
        return cur.GetHandle().AddSucc(newNode);
    }

    template<typename IteratorT>
    Succs<DataT> GetSuccs(IteratorT cur)
    {
        return cur.GetHandle().GetSuccs();
    }

    private:
        using NodeT = Node<DataT>;
        using NodesT = std::deque<NodeT>;
        using HandleT = Handle<DataT>;

        NodeT* AddNode(DataT data, NodeT* pred)
        {
            data_.push_back(std::move(data));
            nodes_.push_back(NodeT(&*data_.rbegin(), pred));
            return &*nodes_.rbegin();
        }
        
        std::deque<DataT> data_;
        NodesT nodes_;
        NodeT* root_;
};

} //tree
#endif //MUGRATREE_LIB
