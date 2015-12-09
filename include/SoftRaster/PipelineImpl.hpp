/// Should never be included directly




template<typename T>
void RuntimeIO::ReadNext(T * data) {
    memcpy(data, inputCacheIter+argInLocs[iterSlotIn++], sizeof(T));
}


template<typename T>
void RuntimeIO::ReadSlot(uint32_t slot, T * data) {
    memcpy(data, inputCacheIter+argInLocs[slot], sizeof(T));
}



template<typename T>
void RuntimeIO::WriteNext(const T * g) {
    memcpy(outputCacheIter+argOutLocs[iterSlotOut++], g, sizeof(T));
}


template<typename T>
void RuntimeIO::WriteSlot(uint32_t slot, const T * g) {
    memcpy(outputCacheIter+argOutLocs[slot], g, sizeof(T));
}

