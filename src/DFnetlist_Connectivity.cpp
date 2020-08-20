#include <algorithm>
#include <cassert>
#include <stack>
#include "../lib/DFnetlist.h"

using namespace Dataflow;
using namespace std;

void DFnetlist_Impl::DFS(bool forward, bool onlyMarked)
{
    // Vector to detect the blocks that have been visited
    // or have been processed.
    vector<bool> visited(blocks.size());

    // All the blocks are not visited, except those that are marked
    ForAllBlocks(b) visited[b] = onlyMarked and not isBlockMarked(b);

    // Visiting order of blocks (first: entry nodes). This is necessary
    // to properly identify the back edges. DFS must start from the
    // entry nodes (see theory in Compilers book)
    listBlocks lBlocks;
    int clock = 0;
    ForAllBlocks(b) {
        setDFSorder(b, -1);
        if (visited[b]) continue;
        if (getBlockType(b) == FUNC_ENTRY) lBlocks.push_front(b);
        else lBlocks.push_back(b);
        ++clock;
    }

    // Now we start an iterative DFS
    DFSorder = vector<blockID>(clock);
    stack<blockID> S, listChildren;

    for (blockID b: lBlocks) {
        if (visited[b]) continue;
        S.push(b);
        visited[b] = true;

        // Put the children to the pending list. The lists of children
        // from different blocks are separated by an invalidDataflowID
        listChildren.push(invalidDataflowID);
        const setPorts& ports = getPorts(b, (forward ? OUTPUT_PORTS : INPUT_PORTS));
        for (portID p: ports) {
            channelID c = getConnectedChannel(p);
            if (onlyMarked and not isChannelMarked(c)) continue; // Skip if the channel is not marked
            blockID other_b = getBlockFromPort(getConnectedPort(p));
            if (not visited[other_b]) listChildren.push(other_b);
        }

        while (not S.empty()) {
            blockID kid = listChildren.top();
            listChildren.pop();
            if (kid == invalidDataflowID) {
                blockID v = S.top();
                S.pop();
                setDFSorder(v, --clock);
            } else {
                if (visited[kid]) continue;
                S.push(kid);
                visited[kid] = true;
                listChildren.push(invalidDataflowID);
                const setPorts& ports = getPorts(kid, (forward ? OUTPUT_PORTS : INPUT_PORTS));
                for (portID p: ports) {
                    channelID c = getConnectedChannel(p);
                    if (onlyMarked and not isChannelMarked(c)) continue; // Skip if the channel is not marked
                    blockID other_b = getBlockFromPort(getConnectedPort(p));
                    if (not visited[other_b]) listChildren.push(other_b);
                }
            }
        }
    }

    assert(clock == 0);

    ForAllBlocks(b) {
        int i = getDFSorder(b);
        assert((onlyMarked and not isBlockMarked(b)) or (i < DFSorder.size()));
        if (i >= 0) DFSorder[i] = b;
    }

}

void DFnetlist_Impl::calculateBackEdges()
{
    ForAllChannels(c) {

        blockID bb = getSrcBlock(c);
        bbID src = getBasicBlock(getSrcBlock(c));
        bbID dst = getBasicBlock(getDstBlock(c));
        // BB ids are given in DFS order from entry
        // If branch goes to its own BB or a BB with a smaller id
        // it is a back edge
        // TODO: determine back edges at CFG level and then apply to CDFG
        if ((getBlockType(bb) == BRANCH) && src >= dst)
            setBackEdge(c, true);
        else
            setBackEdge(c, false); 
    }

    // Forward traversal starting from entry nodes.
    // DFS();

    // Check for back edges
    //ForAllChannels(c) {
    //    int src = getDFSorder(getSrcBlock(c));
    //    int dst = getDFSorder(getDstBlock(c));
    //    if (src < 0 or dst < 0) setBackEdge(c, false);
    //    else setBackEdge(c, src > dst);
    //}
}
