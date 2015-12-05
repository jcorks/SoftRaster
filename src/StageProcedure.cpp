#include <SoftRaster/ShaderProcedure.h>

using namespace SoftRaster;

StageProcedure::SignatureIO(const std::vector<DataType> & l) {
    types = l;
}

void StageProcedure::SignatureIO::AddSlot(DataType in) {
    types.push_back(in);
}

bool StageProcedure::SignatureIO::operator==(const SignatureIO * other) {
    if (types.size() != other->types.size()) return false;
    for(uint32_t i = 0; i < types.size(); ++i) {
        if (types[i] != other->types[i]) return false;
    }
    return true;
}


std::stack<DataType> StageProcedure::SignatureIO::Get() const {
    return std::stack<DataType>(types.begin(), types.end());
}





