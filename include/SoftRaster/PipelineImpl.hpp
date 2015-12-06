/// Should never be included directly




template<typename T>
T * Pipeline::Program::RuntimeIO::ReadNext() {
    return static_cast<T*>(inputCacheIter+argInLocs[iterSlotIn++]);
}


template<typename T>
T * Pipeline::Program::RuntimeIO::ReadSlot(uint32_t slot) {
    return static_cast<T*>(inputCacheIter+argInLocs[slot]);
}



template<typename T>
void Pipeline::Program::RuntimeIO::WriteNext(const T * g) {
    memcpy(outputCacheIter+argOutLocs[iterSlotOut++], g, sizeof(T));
}


template<typename T>
void Pipeline::Program::RuntimeIO::WriteSlot(uint32_t slot, const T * g) {
    memcpy(outputCacheIter+argOutLocs[slot], g, sizeof(T));
}


