#include <iostream>
#include <fstream>
#include <streambuf>

#include "generate.h"

int main(int argc, char *argv[]){
    if (argc < 4){
        return -1;
    }
    int mode = atoi(argv[1]);
    
    VyperFuzz fuzz;

    std::ifstream infile; infile.open(argv[2]);
    std::ofstream outfile; outfile.open(argv[3]);

    std::string input((std::istreambuf_iterator<char>(infile)),
                       std::istreambuf_iterator<char>());

    if (!mode){
        // Vyper ==> Protobuf
        auto contract_proto = fuzz.VyperToProto(input);
        outfile << contract_proto;
    } else {
        // Protobuf ==> Vyper
        auto contract = fuzz.ProtoToVyper(input);
        outfile << contract;
    }

    // vyper::VyperContract *contract;

    // if (!mode){
    //     // Vyper ==> Protobuf
    //     std::string output;
    //     contract = fuzz.VyperToProto(input);
    //     contract->SerializeToString(&output);
    //     outfile << output;
    // } else {
    //     // Protobuf ==> Vyper
    //     // contract = new vyper::VyperContract;
    //     // contract->ParseFromString(input);
    //     // fuzz.mutator.Mutate(contract, 300);
    //     outfile << fuzz.Mutate(input, 400); // fuzz.ProtoToVyper(contract);
    // }

    return 0;
}