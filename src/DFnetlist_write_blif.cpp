#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "../lib/DFnetlist.h"

using namespace Dataflow;
using namespace std;

std::string get_indent_string(int indent) {
    std::string indent_str("");
    for (int i = 0; i < indent; i++)
        indent_str = indent_str + std::string("\t");
    return indent_str;
}

bool DFnetlist_Impl::writeBlif(const string& filename, int indent)
{
    ostringstream of;
    if (not writeBlif(of)) return false;
    return FileUtil::write(of.str(), filename, getError());
}

// TODO: handle indenting
void DFnetlist_Impl::writeSubckt(std::ostream& s, bbID id, int indent) {
    Block& B = blocks[id];
    s << get_indent_string(indent) << "#Node " << B.name << endl;
    if (skipNode(B.type)) {
        s << get_indent_string(indent) << "#Skipped" << endl;
    } else {
        writeNodeType(s, B.type, indent);
        writeNodeInOutPorts(s, id, indent + 1);
    }
}

void DFnetlist_Impl::writeNodeInOutPorts(std::ostream& s, bbID id, int indent) {
    Block& B = blocks[id];
    s << get_indent_string(indent);
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

void DFnetlist_Impl::writeNodeType(std::ostream& s, BlockType t, int indent) {
    //assert(!skipNode(t));
    if (skipNode(t)) return;
    s << get_indent_string(indent) << ".subckt " << BlockType2String[t] << "\\" << endl;
}

bool DFnetlist_Impl::skipNode(BlockType t) {
    return t == FUNC_ENTRY || t == FUNC_EXIT || t == UNKNOWN;
}

void DFnetlist_Impl::writeCircuitInputs(std::ostream &s, int indent) {
    s << get_indent_string(indent) << ".inputs\\\n";
    s << get_indent_string(indent + 1);
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

void DFnetlist_Impl::writeCircuitOutputs(std::ostream &s, int indent) {
    s << get_indent_string(indent) << ".outputs\\\n";
    s << get_indent_string(indent + 1);
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

void DFnetlist_Impl::writeCircuitName(std::ostream &s, int indent) {
    string name = getName();
    if (name.empty()) name = "DataflowNetlist";
    s << get_indent_string(indent) << ".model " << name << endl;
}

bool DFnetlist_Impl::writeBlif(std::ostream& of, int indent) {
    writeCircuitName(of, indent);

    writeCircuitInputs(of, indent + 1);
    writeCircuitOutputs(of, indent + 1);

    ForAllBlocks(b) {
        writeSubckt(of, b, indent + 1);
    }

    of << get_indent_string(indent) << ".end" << endl;
    return true;
}
