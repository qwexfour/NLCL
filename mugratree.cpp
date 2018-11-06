#include "mugratree.hpp"

#include <deque>
#include <algorithm>
#include <cassert>

using namespace tree;

#if 0
NodeBase* tree::IncTreeDFIterator(NodeBase* curNode)
{
    auto& curSuccs = curNode->succs_;
    if (curSuccs.empty())
    {
        // stepping back
        while (curNode->pred_)
        {
            auto pred = curNode->pred_;
            auto& predSuccs = pred->succs_;
            auto nextSuccIt = std::find(predSuccs.begin(), predSuccs.end(), curNode);
            assert(nextSuccIt != predSuccs.end() &&
                "pred must point to current");
            ++nextSuccIt;
            if (nextSuccIt == predSuccs.end())
            {
                // keep stepping back
                curNode = pred;
                continue;
            }
            else
            {
                return *nextSuccIt;
            }
        }
        // reached the end
        return nullptr;
    }
    else
    {
        //continue going deeper
        return curSuccs.front();
    }
}
#endif
