#ifndef _GENERATE_H
#define _GENERATE_H

#include "proto/vyper.pb.h"
#include <src/mutator.h>

class VyperMutator : public protobuf_mutator::Mutator {
    public:
    // nothing here
};

std::string ProtoToVyper(const vyper::VyperContract *contract_proto);

vyper::VyperContract *VyperToProto(const std::string &contract);

#endif