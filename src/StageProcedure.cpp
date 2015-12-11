#include <SoftRaster/StageProcedure.h>

using namespace SoftRaster;

StageProcedure::SignatureIO::SignatureIO(const std::vector<DataType> & l) {
    types = l;
}

void StageProcedure::SignatureIO::AddSlot(DataType in) {
    types.push_back(in);
}

bool StageProcedure::SignatureIO::operator==(const SignatureIO & other) const {
    if (types.size() != other.types.size()) return false;
    for(uint32_t i = 0; i < types.size(); ++i) {
        if (types[i] != other.types[i]) return false;
    }
    return true;
}


std::stack<DataType> StageProcedure::SignatureIO::Get() const {
    
    std::stack<DataType> out;
    if (!types.size()) return out;    
    uint32_t i = types.size();
    while(i) {
        out.push(types[--i]);
    }
    return out;
}





