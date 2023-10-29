// #include "src/mutator.h"
#include "proto/vyper.pb.h"
#include <fmt/format.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>

// TODO impl vyper protobuf mutator
// class VyperMutator : public protobuf_mutator::Mutator {
//     public:
//     // nothing here
// };

std::string CodeBlockToStr(vyper::CodeBlock block){
    // TODO impl parsing block to str
    return "";
}

std::string TypeToStr(vyper::Type type){
    // TODO impl parsing ret to str
    return "";
}

vyper::Type *StrToType(std::string type_tr){
    // TODO impl serialzing str to type
    auto type = new vyper::Type;
    return type;
}

vyper::ArgDef *StrToArgDef(std::string arg_str){
    // TODO impl serializing str to argdef
    auto arg = new vyper::ArgDef;
    return arg;
}

std::string ArgDefToStr(vyper::ArgDef arg){
    // TODO impl parsing argdef to string
    return "";
}

std::string ProtoToVyper(const vyper::VyperContract *contract_proto){
    std::string contract;
    auto functions = contract_proto->functions();
    for (int i = 0; i < functions.size(); i++){

        auto function = functions[i];
        if (function.external()){
            contract = contract.append("@external\n");
        }

        auto args = function.args();
        std::string args_str;
        for (int i = 0; i < args.size(); i++){
            args_str = args_str.append(ArgDefToStr(args[i]));
            if (i != args.size()){
                args_str = args_str.append(", ");
            }
        }

        std::string ret_str = TypeToStr(function.return_type());

        auto block = function.block();

        // TODO impl recursiveness for blocks within blocks
        std::string block_str = CodeBlockToStr(block);

        contract = contract.append(fmt::format("{}({}) -> {}:\n{}\n",
            function.function_name(),
            args_str,
            ret_str,
            block_str
            ));
    }

    return contract;
};

vyper::VyperContract *VyperToProto(const std::string &contract){
    auto contract_proto = new vyper::VyperContract;

    char *ptr = (char *)contract.c_str();
    size_t left = (ptr + contract.size() - ptr);

    while (left){
        bool external;
        char *local_ptr = ptr;

        if (!memcmp(ptr, "@external\n", 10)){
            external = true;
            local_ptr += 10;
        }


        char *next_ptr = (char *)memmem(local_ptr, left, "(", 1);
        size_t name_len = next_ptr - local_ptr;
        char name[name_len + 1];
        memset(&name, 0, name_len + 1);
        memcpy(&name, local_ptr, name_len);
        local_ptr = next_ptr + 1;
        std::string name_str(name);

        next_ptr = (char *)memmem(local_ptr, left, ") -> ", 5);
        size_t args_len = next_ptr - local_ptr;
        char args[args_len + 1];
        memset(&args, 0, args_len + 1);
        memcpy(&args, local_ptr, args_len);
        local_ptr = next_ptr + 5;
        // TODO parse args

        next_ptr = (char *)memmem(local_ptr, left, ":\n", 2);
        size_t ret_len = next_ptr - local_ptr;
        char ret[ret_len + 1];
        memset(&ret, 0, ret_len + 1);
        memcpy(&ret, local_ptr, ret_len);
        local_ptr = next_ptr + 2;

        auto function = contract_proto->add_functions();
        function->set_external(external);
        function->set_function_name(name_str);

        external = false;
        left -= (local_ptr - ptr);
        ptr = local_ptr;

        // TODO parse code blockS
    }

    return contract_proto;
}

int main(int argc, char *argv[]){
    if (argc < 4){
        return -1;
    }
    int mode = atoi(argv[1]);
    
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
        outfile << ProtoToVyper(contract);
    }

    return 0;
}