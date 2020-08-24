#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "../lib/DFnetlist.h"

using namespace Dataflow;
using namespace std;


bool DFnetlist_Impl::writeBlif(const string& filename)
{
    ostringstream of;
    if (not writeBlif(of)) return false;
    return FileUtil::write(of.str(), filename, getError());
}

// TODO: handle indenting
void DFnetlist_Impl::writeSubckt(std::ostream& s, bbID id) {
    Block& B = blocks[id];
    s << "#Node " << B.name << endl;
    if (writeNodeType(s, B.type)) {

    }
}

bool DFnetlist_Impl::writeNodeType(std::ostream& s, BlockType t) {

    if (t == FUNC_ENTRY || t == FUNC_EXIT || t == UNKNOWN) {
        s << "#Skipped" << endl;
        return false;
    }
    s << ".subckt " << BlockType2String[t] << "\\" << endl;
    return true;
}


bool DFnetlist_Impl::writeBlif(std::ostream& of) {
    // TODO: add code for writing circuit name + input/outputs

    // TODO: probably need to do a bit of preprocessing here :-?

    ForAllBlocks(b) {
        writeSubckt(of, b);
    }
    return true;
}
