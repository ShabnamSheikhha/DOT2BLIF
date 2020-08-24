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
        writeNodeInOutPorts(s, id);
    }
}

void DFnetlist_Impl::writeNodeInOutPorts(std::ostream& s, bbID id) {
    Block& B = blocks[id];
    for (auto& inP: B.inPorts) {
        Port& P = ports[inP];
        s << P.short_name << "=";
        writePortChannel(s, P.channel);
        s << "\t";
    }

    for (auto& outP: B.outPorts) {
        Port& P = ports[outP];
        s << P.short_name << "=";
        writePortChannel(s, P.channel);
        s << "\t";
    }
    s << endl;
}

void DFnetlist_Impl::writePortChannel(std::ostream& s, channelID id) {
    Channel C = channels[id];

    Port tailPort = ports[C.src];
    Port headPort = ports[C.dst];
    Block tailBlock = blocks[tailPort.block];
    Block headBlock = blocks[headPort.block];

    cout << "TEST:" << tailPort.full_name << endl;
    s << tailBlock.name << "." << tailPort.short_name << "*" << BlockType2String[tailBlock.type] << "*" << "~";
    s << headBlock.name << "." << headPort.short_name << "*" << BlockType2String[headBlock.type] << "*";
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
