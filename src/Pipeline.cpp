#include <SoftRaster/Pipeline.h>

using namespace SoftRaster;

const char * p_error_c__begin_signature_mismatch = "ERROR: The first ShaderProcedure must accept just one UserVertex as its input.";
const char * p_error_c__end_signature_mismatch   = "ERROR: The last ShaderProcedure must not return anything";
const char * p_error_c__io_mismatch              = "ERROR: The current end of the pipeline's return signature does not match the incoming ShaderProcedure's input signature."

const uint32_t pipeline_program_init_cache_size     = 1024 * 512; // 512 KB should be fine to start
const float    pipeline_program_cache_resize_factor = 1.1f;

static uint32_t SignatureSize(const SignatureIO &, uint32_t sizeofVertex);

std::string Pipeline::PushExecutionStage(ShaderProcedure * proc) {
    const StageProcedure::SignatureIO pipelineHead (
        DataType::UserVertex;
    );
    if (procs.empty()) {
        // the first stage only accepts a user vertex
        if (porc->InputSignature() != pipelineHead) {
            return p_error_c__begin_signature_mismatch;
        }
    } else {

        // check that io connects properly.
        // (would be better if it was completely type aware, but...)
        if (procs[procs.size()-1]->OutputSignature() !=
                             proc->InputSignature()) {
            return p_error_c__io_mismatch;
        }   
    }

    procs.push_back(proc);
    return "";

}


Pipeline::Program * Pipeline::Compile() {
    if (!procs.size()) return nullptr;
    if (procs[procs.size()-1]->OutputSignature != StageProcedure::SignatureIO()) {
        return nullptr;
    }
    Program * out = new Program("");
    for(int i = 0; i < procs.size(); ++i) {
        out->cachedProcs.push_back(procs[i]);
    }

    return out;
}

void Pipeline::Clear() {
    procs.clear();
}






Pipeline::Program::Program(const std::string s) {
    status = s;
    src = nullptr;
}


std::string Pipeline::Program::GetStatus() {
    return status;
}

void Pipeline::Program::Run(
        Texture * framebuffer, 
        uint8_t * v, 
        uint32_t sizeofVertex,
        uint32_t num) {
    uint32_t currentSize;


    std::stack<DataType> input;
    static RuntimeIO runtimeIO;
    




    runtimeIO.RunSetup(v, sizeofVertex, num);

    for(uint32_t i = 1; i < cachedProcs.size(); ++i) {
        runtimeIO.NextProc(cachedProcs[i]);


        for(uint32_t n = 0; n < num; ++n) {
            cachedProcs[i](&runtimeIO);
            runtimeIO.NextIter();
        }

    }
    

}


/////// RuntimeIO
Pipeline::Program::RuntimeIO::RuntimeIO {
    inputCacheSize = pipeline_program_init_cache_size;
    outputCacheSize = pipeline_program_init_cache_size;
    inputCache = new uint8_t[inputCacheSize];
    outputCache = new uint8_t[outputCacheSize];
}


void Pipeline::Program::RuntimeIO::RunSetup(
    uint8_t * vData,
    uint32_t szVertex,
    uint32_t numIteration
) {
    sizeofVertex = szVertex;
    commitCount = 0;

    PrepareOutputCache(sxVertex*numIteraton);
    memcpy(outputCache, vData, sxVertex*numIteraton);
}


void Pipeline::Program::RuntimeIO::NextProc(const StageProcedure * p) {
    iterIn = 0;
    iterOut = 0;


    // since we are dealing with a new proc,
    // we need to correct our io signatures
    argSizeIn.clear();
    argSizeOut.clear();

    auto input  = p->InputSignature();
    auto output = p->OutputSignature();
   
    inputSize  = SignatureSize(p->InputSignature());
    outputSize = SignatureSize(p->OutputSignature());

    auto iStack = input.Get();
    while(!iStack.empty()) {
        argSizeIn.push_back(SizeOf(iStack.top())); iStack.pop();
    }
    auto oStack = output.Get();
    while(!oStack.empty()) {
        argSizeOut.push_back(SizeOf(oStack.top())); oStack.pop();
    }



    // Reset the proc

    procIterCount   = commitCount;
    currentProcIter = 0;
    commitCount     = 0;

    PrepareInputCache(procIterCount * (inputSize));
    PrepareOutputCache(procIterCount * (inputSize));
    inputCache.swap(outputCache);
}



void Pipeline::Program::RuntimeIO::NextIter() {
    currentProcIter++;
    // want to reset the read iter in acse user didnt
    // actually read in some args
    iterIn  = currentProcIter*inputSize;
}


void Pipeline::Program::RuntimeIO::PrepareInputCache(uint32_t b) {
    if (b < inputCacheSize) return;
    inputCacheSize*=pipeline_program_cache_resize_factor;
    inputCache = realloc(inputCache, inputCacheSize);
}

void Pipeline::Program::RuntimeIO::PrepareOutputCache(uint32_t b) {
    if (b < outputCacheSize) return;
    outputCacheSize*=pipeline_program_cache_resize_factor;
    outpuCache = realloc(outputCache, outputCacheSize);
}

void Pipeline::Program::RuntimeIO::Commit() {
    commitCount++;
    PrepareOutputCache((commitCount+1)*outputSize);
}








///// Statics//////
uint32_t SignatureSize(const SignatureID & sig, uint32_t sizeofVertex) {

    std::stack<DataType> stk = sig.Get();
    uint32_t size = 0;
    while(!std.empty()) {
        switch(stk.top()) {
            case DataType::Null: break; 
            case DataType::Float:   size += sizeof(float);   break;
            case DataType::Int:     size += sizeof(int);     break;
            case DataType::Vertex2: size += sizeof(Vertex2); break;
            case DataType::Vertex3: size += sizeof(Vertex3); break;
            case DataType::Vertex4: size += sizeof(Vertex4); break;
            case DataType::Mat4:    size += sizeof(Mat4);    break;
            case DataType::UserVertex:
                size += sizeofVertex;
            default: assert("Could not determine size of variable..");
            break;
        }
    }
    return size;
}





