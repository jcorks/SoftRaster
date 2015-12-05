/// Should never be included directly




template<typename T>
T * Pipeline::Program::RuntimeIO::ReadNext<T>() {
    return static_cast<T*>(inputCache+argInLocs[iterSlotIn++]);
}


template<typename T>
T * Pipeline::Program::RuntimeIO::ReadSlot<T>(uint32_t slot) {
    return static_cast<T*>(inputCache+argInLocs[slot]);
}



template<typename T>
T * Pipeline::Program::RuntimeIO::WriteNext<T>(T * g) {
    return static_cast<T*>(inputCache+argInLocs[iterSlotIn++]);
}


template<typename T>
T * Pipeline::Program::RuntimeIO::ReadSlot<T>(uint32_t slot) {
    return static_cast<T*>(inputCache+argInLocs[slot]);
}


