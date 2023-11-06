#ifndef _GENERATE_H
#define _GENERATE_H

#include "proto/vyper.pb.h"
#include <src/mutator.h>

std::string ValOrOpToStr(vyper::ValOrOp val);

std::string FunctionCallToStr(vyper::FunctionCall func_call);

std::string BuiltinCallToStr(vyper::BuiltinCall call);

class VyperMutator : public protobuf_mutator::Mutator {
    public:
    // nothing here
};

class VyperFuzz {
    public:
        VyperMutator mutator;
        std::string ProtoToVyper(const std::string contract_proto); // Returns a Vyper contract from a serialized protobuf
        std::string VyperToProto(const std::string contract); // Returns a serialized protobuf from a Vyper contract
        std::string Mutate(const std::string input, size_t max_size_hint); // Returns a mutated serialized protobuf from a serialized protobuf
};

#endif