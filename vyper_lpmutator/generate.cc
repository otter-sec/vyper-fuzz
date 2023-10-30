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
    auto desc = vyper::TYPE_ENUM_descriptor();
    auto val = desc->FindValueByNumber(type.typeval())->name();
    return val;
}

vyper::Type *StrToType(std::string type_str){
    auto type = new vyper::Type;
    auto desc = vyper::TYPE_ENUM_descriptor();
    auto val = desc->FindValueByName(type_str)->number();
    type->set_typeval(vyper::TYPE_ENUM(val)); 
    return type;
}

vyper::ArgDef *StrToArgDef(std::string arg_str){
    auto arg = new vyper::ArgDef;
    char *name_ptr = (char *)arg_str.c_str();
    size_t size = arg_str.size();
    char *type_ptr = (char *)memmem(name_ptr, size, ": ", 2) + 2;

    size_t name_len = type_ptr - name_ptr - 2;
    char name[name_len + 1];
    memset(&name, 0, name_len + 1);
    memcpy(&name, name_ptr, name_len);
    std::string name_str(name);
    std::cout << "name_str: " << name_str << std::endl;

    size_t type_len = size - name_len;
    char type[type_len + 1];
    memset(&type, 0, type_len + 1);
    memcpy(&type, type_ptr, type_len);
    std::string type_str(type);
    std::cout << "type_str: " << type_str << std::endl;
    auto type_obj = StrToType(type_str);

    arg->set_allocated_arg_type(type_obj);
    arg->set_arg_name(name_str);
    std::cout << "return arg" << std::endl;
    return arg;
}

std::vector<vyper::ArgDef *> StrToArgDefs(std::string args_str){
    // TODO impl serializing str to argdef
    std::vector<vyper::ArgDef *> args;

    char *ptr = (char *)args_str.c_str();
    size_t left = args_str.size();
    size_t count = 0;
    std::cout << args_str << std::endl;
    while (left){
        std::cout << left << std::endl;
        char *next_ptr = (char *)memmem(ptr, left, ",", 1);
        size_t arg_size;
        if(next_ptr){
            arg_size = next_ptr - ptr;
        } else {
            std::cout << "last case" << std::endl;
            arg_size = left;
        }
        char arg[arg_size + 1];
        memset(&arg, 0, arg_size + 1);
        memcpy(&arg, ptr, arg_size);
        std::string arg_str(arg);
        std::cout << arg_str << std::endl;
        std::cout << "enter StrToArgDef" << std::endl;
        args.push_back(StrToArgDef(arg_str));
        std::cout << "passed StrToArgDef" << std::endl;
        ptr += arg_size + 1;
        left -= arg_size;
    }

    return args;
}

std::string ArgDefToStr(vyper::ArgDef arg){
    // TODO impl parsing argdef to string

    return fmt::format("{}: {}", arg.arg_name(), TypeToStr(arg.arg_type()));
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
            if (i != args.size() -1){
                args_str = args_str.append(",");
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
    size_t left = contract.size();

    while (left){
        bool external;
        char *local_ptr = ptr;

        if (!memcmp(ptr, "@external\n", 10)){
            external = true;
            local_ptr += 10;
        }

        //TODO parse def statement in front of function definition
        // Parse function name
        char *next_ptr = (char *)memmem(local_ptr, left, "(", 1);
        size_t name_len = next_ptr - local_ptr;
        char name[name_len + 1];
        memset(&name, 0, name_len + 1);
        memcpy(&name, local_ptr, name_len);
        local_ptr = next_ptr + 1;
        std::string name_str(name);

        // Parse arguments
        next_ptr = (char *)memmem(local_ptr, left, ") -> ", 5);
        size_t args_len = next_ptr - local_ptr;
        char args_str[args_len + 1];
        memset(&args_str, 0, args_len + 1);
        memcpy(&args_str, local_ptr, args_len);
        local_ptr = next_ptr + 5;
        std::cout << "enter StrArgDefs" << std::endl;
        auto args = StrToArgDefs(args_str);
        std::cout << "passed StrArgDefs" << std::endl;

        // Parse Return type
        next_ptr = (char *)memmem(local_ptr, left, ":\n", 2);
        size_t ret_len = next_ptr - local_ptr;
        char ret[ret_len + 1];
        memset(&ret, 0, ret_len + 1);
        memcpy(&ret, local_ptr, ret_len);
        local_ptr = next_ptr + 2;
        std::string ret_str(ret);
        auto ret_type = StrToType(ret_str);

        // Assemble proto
        auto function = contract_proto->add_functions();
        function->set_external(external);
        function->set_function_name(name_str);
        function->set_allocated_return_type(ret_type);

        for (int i = 0; i < args.size(); i++){
            auto arg = function->add_args();
            arg->set_arg_name(args[i]->arg_name());
            arg->set_allocated_arg_type((vyper::Type *)&args[i]->arg_type());
        }

        external = false;
        left -= (local_ptr - ptr);
        ptr = local_ptr;

        // TODO parse code blocks
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