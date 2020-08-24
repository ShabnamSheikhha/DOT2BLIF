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

    // TODO: check the blif manual to see if ":" is allowed in names.
    //  if so, instead of all this, you can just use port.full_name
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

void DFnetlist_Impl::writeCircuitInputs(std::ostream &s) {
    s << ".inputs\\\n";
    ForAllBlocks(b) {
        Block B = blocks[b];
        if (B.type == FUNC_ENTRY) {
            for (auto& p: B.outPorts) {
                writePortChannel(s, ports[p].channel);
                s << " ";
            }
        }
    }
    s << endl;
}

void DFnetlist_Impl::writeCircuitOutputs(std::ostream &s) {
    s << ".output\\\n";
    ForAllBlocks(b) {
        Block B = blocks[b];
        if (B.type == FUNC_EXIT) {
            for (auto& p: B.inPorts) {
                writePortChannel(s, ports[p].channel);
                s << " ";
            }
        }
    }
    s << endl;
}

void DFnetlist_Impl::writeCircuitName(std::ostream &s) {
    string name = getName();
    if (name.empty()) name = "DataflowNetlist";
    s << ".model " << name << endl;
}

bool DFnetlist_Impl::writeBlif(std::ostream& of) {
    writeCircuitName(of);
    writeCircuitInputs(of);
    writeCircuitOutputs(of);

    ForAllBlocks(b) {
        writeSubckt(of, b);
    }
    of << ".end" << endl;
    return true;
}
