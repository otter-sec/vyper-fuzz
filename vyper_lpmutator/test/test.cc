#include <iostream>
#include <fstream>
#include <streambuf>

#include "generate.h"

int main(int argc, char *argv[]){
    if (argc < 4){
        return -1;
    }
    int mode = atoi(argv[1]);
    
    VyperMutator mutator;

    std::ifstream infile; infile.open(argv[2]);
    std::ofstream outfile; outfile.open(argv[3]);

    std::string input((std::istreambuf_iterator<char>(infile)),
                       std::istreambuf_iterator<char>());

    vyper::VyperContract *contract;

    if (!mode){
        // Vyper ==> Protobuf
        std::string output;
        contract = VyperToProto(input);
        contract->SerializeToString(&output);
        outfile << output;
    } else {
        // Protobuf ==> Vyper
        contract = new vyper::VyperContract;
        contract->ParseFromString(input);
        mutator.Mutate(contract, 300);
        outfile << ProtoToVyper(contract);
    }

    return 0;
}