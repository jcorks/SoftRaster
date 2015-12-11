/// Should never be included directly

#ifdef SOFTRASTER_RT_CHECKS
    #include <sstream>
    #include <iostream>
    #include <string>
    inline void SR_RT_DEBUG_print(bool warn, bool read, uint32_t iter, uint32_t iterCount, const std::string & s) {
        std::cout << "[SoftRaster]";
        if (warn) std::cout << "[ WARNING ]";
        else      std::cout << "[! ERROR !]";

        std::cout << (read? "(I" : "(O");
        std::cout << ":Iter " << iter << "\t/" << iterCount << "\t)   ";


        std::cout << s << std::endl;
            
    }
#endif

namespace SoftRaster {




template<typename T>
void RuntimeIO::ReadNext(T * data) {
  #ifdef SOFTRASTER_RT_CHECKS
    if (iterSlotIn > argInLocs.size())
        SR_RT_DEBUG_print(false, true, currentProcIter, procIterCount, "ReadNext| read past slot count promised by InputSignature()"); 
    
    if (sizeof(T) != argInLocs[iterSlotIn+1] - argInLocs[iterSlotIn]) {
        SR_RT_DEBUG_print(true, true, currentProcIter, procIterCount, "ReadNext| Sizeof type does not match registered type slot size.");
    }
  #endif
    memcpy(data, inputCacheIter+argInLocs[iterSlotIn++], sizeof(T));
}


template<typename T>
void RuntimeIO::ReadSlot(uint32_t slot, T * data) {
  #ifdef SOFTRASTER_RT_CHECKS
    if (slot > argInLocs.size())
        SR_RT_DEBUG_print(false, true, currentProcIter, procIterCount, "ReadSlot| slot is invalid"); 


    if (sizeof(T) != argInLocs[slot+1] - argInLocs[slot]) {
        SR_RT_DEBUG_print(true, true, currentProcIter, procIterCount, "ReadSlot| Sizeof type does not match registered type slot size.");
    }
  #endif
    memcpy(data, inputCacheIter+argInLocs[slot], sizeof(T));
}



template<typename T>
void RuntimeIO::WriteNext(const T * g) {
  #ifdef SOFTRASTER_RT_CHECKS
    if (iterSlotOut > argOutLocs.size())
        SR_RT_DEBUG_print(false, false, currentProcIter, procIterCount, "WriteNext| read past slot count promised by InputSignature()"); 
    
    if (sizeof(T) != argOutLocs[iterSlotOut+1] - argOutLocs[iterSlotOut]) {
        SR_RT_DEBUG_print(true, false, currentProcIter, procIterCount, "WriteNext| Sizeof type does not match registered type slot size.");
    }
  #endif
    memcpy(outputCacheIter+argOutLocs[iterSlotOut++], g, sizeof(T));
}


template<typename T>
void RuntimeIO::WriteSlot(uint32_t slot, const T * g) {
  #ifdef SOFTRASTER_RT_CHECKS
    if (slot > argOutLocs.size())
        SR_RT_DEBUG_print(false, false, currentProcIter, procIterCount, "WriteSlot| read past slot count promised by InputSignature()"); 
    
    if (sizeof(T) != argOutLocs[slot+1] - argOutLocs[slot]) {
        SR_RT_DEBUG_print(true, false, currentProcIter, procIterCount, "WriteSlot| Sizeof type does not match registered type slot size.");
    }
  #endif
    memcpy(outputCacheIter+argOutLocs[slot], g, sizeof(T));
}

}
