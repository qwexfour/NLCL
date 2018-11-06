#ifndef MUGRATREE_LIB
#define MUGRATREE_LIB

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <variant>

namespace tree
{

template<typename DataT, template<typename ...> typename Seq>
struct Node;

template<typename DataT, template<typename ...> typename Seq>
struct Succs;

template<typename DataT, template<typename ...> typename Seq>
struct SuccsIterator;

template<typename DataT, template<typename ...> typename Seq>
struct DFIterator;

template<typename DataT, template<typename ...> typename Seq>
struct TreeIterator;

template<typename DataT, template<typename ...> typename Seq>
struct Handle;

template<typename DataT, template<typename ...> typename Seq>
struct DF;

template<typename DataT, template<typename ...> typename Seq>
struct DFIterator;

template<typename DataT, template<typename ...> typename Seq = std::deque>
struct TypeLib
{
    using DataSeq = Seq<DataT>;
    using DataIt = typename DataSeq::iterator;
    using NodeT = Node<DataT, Seq>;
    using NodeSeq = Seq<NodeT>;
    using NodeIt = typename NodeSeq::iterator;
    using NodeItSeq = Seq<NodeIt>;
    using NodeItIt = typename NodeItSeq::iterator;
    using TreeIteratorT = TreeIterator<DataT, Seq>;
    using SuccsIteratorT = SuccsIterator<DataT, Seq>;
    using SuccsT = Succs<DataT, Seq>;
    using HandleT = Handle<DataT, Seq>;
    using DFT = DF<DataT, Seq>;
    using DFIteratorT = DFIterator<DataT, Seq>;
};

template<typename DataT, template<typename ...> typename Seq>
struct Node
{
    using Type = TypeLib<DataT, Seq>;
    
    Node(typename Type::DataIt dataIt) : dataIt_(dataIt) {}
    
    typename Type::DataIt dataIt_;
    typename Type::NodeIt predIt_;
    typename Type::NodeItSeq succItSeq_;

    void Dump()
    {
        std::cout << *dataIt_ << std::endl;
    }
};

template<typename DataT, template<typename ...> typename Seq>
struct Succs
{
    using Type = TypeLib<DataT, Seq>;
    
    Succs(typename Type::NodeItSeq& succItSeq) : succItSeq_(succItSeq) {}

    typename Type::SuccsIteratorT begin()
    {
        return typename Type::SuccsIteratorT(succItSeq_.begin());
    }

    typename Type::SuccsIteratorT end()
    {
        return typename Type::SuccsIteratorT(succItSeq_.end());
    }

    private:
        typename Type::NodeItSeq& succItSeq_;
};

template<typename DataT, template<typename ...> typename Seq>
struct SuccsIterator
{
    using Type = TypeLib<DataT, Seq>;

    SuccsIterator(typename Type::NodeItIt succItIt) :
        succItIt_(succItIt) {}

    DataT& operator*() const
    {
        return *((*succItIt_)->dataIt_);
    }

    DataT* operator->() const
    {
        return (*succItIt_)->dataIt_;
    }

    SuccsIterator& operator++()
    {
        ++succItIt_;
        return *this;
    }

    SuccsIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    SuccsIterator& operator--()
    {
        --succItIt_;
        return *this;
    }

    SuccsIterator operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    typename Type::HandleT GetHandle()
    {
        return typename Type::HandleT(*succItIt_);
    }

    friend bool operator==(const SuccsIterator& lhs, const SuccsIterator& rhs)
    {
        return lhs.succItIt_ == rhs.succItIt_;
    }

    friend bool operator!=(const SuccsIterator& lhs, const SuccsIterator& rhs)
    {
        return lhs.succItIt_ != rhs.succItIt_;
    }

    private:
        typename Type::NodeItIt succItIt_;
};

template<typename DataT, template<typename ...> typename Seq>
struct Handle
{
    using Type = TypeLib<DataT, Seq>;
    
    Handle(typename Type::NodeIt nodeIt) : nodeIt_(nodeIt) {}

    typename Type::TreeIteratorT GetPred()
    {
        return typename Type::TreeIteatorT(nodeIt_->predIt_);
    }

    typename Type::SuccsT GetSuccs()
    {
        return typename Type::SuccsT(nodeIt_->succItSeq_);
    }

    typename Type::TreeIteratorT AddSucc(typename Type::NodeIt newNode)
    {
        newNode->predIt_ = nodeIt_;
        nodeIt_->succItSeq_.push_back(newNode);
        return typename Type::TreeIteratorT(newNode);
    }

    typename Type::TreeIteratorT DeleteLeaf(typename Type::DataSeq& dataSeq, typename Type::NodeSeq& nodeSeq)
    {
        assert(nodeIt_->succItSeq_.empty() &&
            "I've said Leaf");
        auto pred = nodeIt_->predIt_;
        auto& predSuccs = pred->succItSeq_;
        std::remove(predSuccs.begin(), predSuccs.end(), nodeIt_);
        dataSeq.erase(nodeIt_->dataIt_);
        nodeSeq.erase(nodeIt_);
        return pred;
    }

    private:
        typename Type::NodeIt nodeIt_;
};

template<typename DataT, template<typename ...> typename Seq>
struct DF
{
    using Type = TypeLib<DataT, Seq>;

    DF(typename Type::NodeSeq& nodeSeq, typename Type::NodeIt root) :
        nodeSeq_(nodeSeq), root_(root) {}

    typename Type::DFIteratorT begin()
    {
        return typename Type::DFIteratorT(root_, nodeSeq_.end());
    }

    typename Type::DFIteratorT end()
    {
        return typename Type::DFIteratorT(nodeSeq_.end(), nodeSeq_.end());
    }

    private:
        typename Type::NodeSeq& nodeSeq_;
        typename Type::NodeIt root_;
};

template<typename DataT, template<typename ...> typename Seq>
struct DFIterator
{
    using Type = TypeLib<DataT, Seq>;

    explicit DFIterator(typename Type::NodeIt nodeIt, typename Type::NodeIt end) :
        nodeIt_(nodeIt), end_(end) {}
    
    DataT& operator*() const
    {
        return *(nodeIt_->dataIt_);
    }

    DataT* operator->() const
    {
        return nodeIt_->dataIt_;
    }

    DFIterator& operator++()
    {
        auto curNodeIt = nodeIt_;
        auto& curSuccItSeq = curNodeIt->succItSeq_;
        if (curSuccItSeq.empty())
        {
            // stepping back
            // until root (root's pred is root)
            while (curNodeIt->predIt_ != curNodeIt)
            {
                auto pred = curNodeIt->predIt_;
                auto& predSuccs = pred->succItSeq_;
                auto nextSuccIt = std::find(predSuccs.begin(), predSuccs.end(), curNodeIt);
                assert(nextSuccIt != predSuccs.end() &&
                    "pred must point to current");
                ++nextSuccIt;
                if (nextSuccIt == predSuccs.end())
                {
                    // keep stepping back
                    curNodeIt = pred;
                    continue;
                }
                else
                {
                    // stop stepping back
                    // start going deeper again
                    nodeIt_ = *nextSuccIt;
                    return *this;
                }
            }
            // reached the end
            nodeIt_ = end_;
            return *this;
        }
        else
        {
            //continue going deeper
            nodeIt_ = curSuccItSeq.front();
            return *this;
        }
    }

    DFIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    typename Type::HandleT GetHandle()
    {
        return typename Type::HandleT(nodeIt_);
    }

    friend bool operator==(const DFIterator& lhs, const DFIterator& rhs)
    {
        return lhs.nodeIt_ == rhs.nodeIt_;
    }

    friend bool operator!=(const DFIterator& lhs, const DFIterator& rhs)
    {
        return lhs.nodeIt_ != rhs.nodeIt_;
    }

    private:
        typename Type::NodeIt nodeIt_;
        typename Type::NodeIt end_;
};

template<typename DataT, template<typename ...> typename Seq>
struct TreeIterator
{
    using Type = TypeLib<DataT, Seq>;

    TreeIterator(typename Type::NodeIt nodeIt) : nodeIt_(nodeIt) {}

    DataT& operator*() const
    {
        return *nodeIt_->dataIt_;
    }

    DataT* operator->() const
    {
        return nodeIt_->dataIt_;
    }

    TreeIterator& operator++()
    {
        ++nodeIt_;
        return *this;
    }

    TreeIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    TreeIterator& operator--()
    {
        --nodeIt_;
        return *this;
    }

    TreeIterator operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    typename Type::HandleT GetHandle()
    {
        return typename Type::HandleT(nodeIt_);
    }

    friend bool operator==(const TreeIterator& lhs, const TreeIterator& rhs)
    {
        return lhs.nodeIt_ == rhs.nodeIt_;
    }

    friend bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs)
    {
        return lhs.nodeIt_ != rhs.nodeIt_;
    }

    private:
        typename Type::NodeIt nodeIt_;
};

template<typename DataT, template<typename ...> typename Seq = std::deque>
struct Tree
{
    using Type = TypeLib<DataT, Seq>;
    
    Tree()
    {
        root_ = nodeSeq_.end();
    }
    
    explicit Tree(DataT data)
    {
        root_ = AddNode(std::move(data));
    }

    typename Type::TreeIteratorT SetRoot(DataT data)
    {
        if (root_ == nodeSeq_.end())
        {
            root_ = AddNode(std::move(data));
        }
        else
        {
            *(root_->dataIt_) = std::move(data);
        }

        return typename Type::TreeIteratorT(root_);
    }

    typename Type::TreeIteratorT begin()
    {
        return typename Type::TreeIteratorT(nodeSeq_.begin());
    }

    typename Type::TreeIteratorT end()
    {
        return typename Type::TreeIteratorT(nodeSeq_.end());
    }

    typename Type::TreeIteratorT GetRoot()
    {
        return typename Type::TreeIteratorT(root_);
    }

    typename Type::DFT GetDF()
    {
        return typename Type::DFT(nodeSeq_, root_);
    }

    template<typename IteratorT>
    typename Type::TreeIteratorT GetPred(IteratorT cur)
    {
        return cur.GetHandle().GetPred();
    }

    template<typename IteratorT>
    typename Type::TreeIteratorT AddSucc(DataT data, IteratorT cur)
    {
        auto newNodeIt = AddNode(std::move(data));
        return cur.GetHandle().AddSucc(newNodeIt);
    }

    template<typename IteratorT>
    typename Type::SuccsT GetSuccs(IteratorT cur)
    {
        return cur.GetHandle().GetSuccs();
    }

    template<typename IteratorT>
    typename Type::TreeIteratorT DeleteLeaf(IteratorT cur)
    {
        return cur.GetHandle().DeleteLeaf(dataSeq_, nodeSeq_);
    }

    private:
        // initializes:
        //   - dataIt_ field
        //   - predIt_ field with iterator to self
        // succItSeq_ remains empty
        typename Type::NodeIt AddNode(DataT data)
        {
            dataSeq_.push_back(std::move(data));
            typename Type::DataIt dataIt = --dataSeq_.end();
            nodeSeq_.push_back(typename Type::NodeT(dataIt));
            typename Type::NodeIt nodeIt = --nodeSeq_.end();
            nodeIt->predIt_ = nodeIt;
            return nodeIt;
        }

        typename Type::DataSeq dataSeq_;
        typename Type::NodeSeq nodeSeq_;
        typename Type::NodeIt root_;
};

template<typename ...Args>
using LasyMuGraTree = Tree<std::variant<Args...>>;

} //tree
#endif //MUGRATREE_LIB
