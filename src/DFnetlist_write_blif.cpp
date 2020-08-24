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

bool DFnetlist_Impl::writeBlif(std::ostream& of) {
    cout << "writing stuff for blif!" << endl;
    return true;
}
