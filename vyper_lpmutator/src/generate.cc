#include "generate.h"
#include <fmt/format.h>
#include <string>

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

enum Builtins {
    concat,
    convert
};

static std::unordered_map<std::string,Builtins> const func_table = {
    {"concat",Builtins::concat},
    {"convert",Builtins::convert}
};

std::string ValueToStr(vyper::Value val){
    std::string val_str;

    if (val.has_num()){
        // TODO impl num val to str
    } else if (!val.str_literal().empty()){
        val_str = fmt::format("\" {} \"", val.str_literal());
    } else if (!val.var_name().empty()){
        val_str = val.var_name();
    }

    return val_str;
}

std::string ValOrOpToStr(vyper::ValOrOp val){
    std::string val_str;

    if(val.has_op()){
        // TODO impl op to str
    } else if (val.has_val()){
        val_str = ValueToStr(val.val());
    }

    return val_str;
}

std::string StatementToLine(vyper::Statement statement){
    std::string statement_str;

    if (statement.has_assignment()){
        // TODO impl assignment to line
    } else if (statement.has_builtin_call()){
        // TODO impl builtin call to line
    } else if (statement.has_function_call()){
        std::cout << "has function call" << std::endl;
        auto func_call = statement.function_call();
        auto func_args = func_call.args();
        std::string args_str;

        for (int i = 0; i < func_args.size(); i++){
            args_str = args_str.append(ValOrOpToStr(func_args[i]));
            if (i != func_args.size() -1){
                args_str = args_str.append(",");
            }
            statement_str = fmt::format("{}({})", func_call.function_name(), args_str);
            std::cout << statement_str << std::endl;
        }
        
    } else if (statement.has_return_()){
        // TODO impl return to line
    }

    return statement_str;
}

std::string CodeBlockToStr(vyper::CodeBlock block){
    std::string block_str;

    auto statements = block.statements();
    for (int i = 0; i < statements.size(); i++){
        std::cout << "Enter statement to line" << std::endl;
        block_str = block_str.append(StatementToLine(statements[i]));
    }

    return block_str;
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
    return fmt::format("{}: {}", arg.arg_name(), TypeToStr(arg.arg_type()));
}

std::vector<std::string> GetBlockLines(char **ptr, size_t left){
    std::vector<std::string> lines;
    while (1){
        std::cout << "GetBlockLines loop" << std::endl;
        if(memcmp(*ptr, "    ", 4)){
            std::cout << "not in block" << std::endl;
            break;
        }
        char *start_ptr = *ptr + 4;
        char *end_ptr = (char *)memmem(start_ptr, left - 4, "\n", 1);
        size_t line_len = end_ptr - start_ptr;
        char line[line_len + 1];
        memset(&line, 0, line_len + 1);
        memcpy(&line, start_ptr, line_len);
        std::string line_str(line);
        std::cout << "line: " << line_str << std::endl;
        lines.push_back(line);
        *ptr += line_len + 5;
    };
    std::cout << "returning lines" << std::endl;
    return lines;
}

std::vector<std::string> ArgsSplit(std::string args){
    std::vector<std::string> arg_vec; 
    char *ptr = (char *)args.c_str();
    size_t left = args.size();

    while(left){
        std::cout << left << std::endl;
        char *next_ptr = (char *)memmem(ptr, left, ",", 1);
        size_t arg_len;
        if (next_ptr){
            arg_len = next_ptr - ptr;
        } else {
            arg_len = left;
        }
        char arg[arg_len + 1];
        memset(&arg, 0, arg_len +1);
        memcpy(&arg, ptr, arg_len);
        std::string arg_str(arg);
        arg_vec.push_back(arg_str);
        ptr = next_ptr + 1;

        left -= arg_len;
    }

    return arg_vec;
}

void StrToValOrOp(std::string arg_str, vyper::ValOrOp *arg){
    auto val = new vyper::Value;

    // TODO impl for op

    if (is_number(arg_str)){
        auto num = new vyper::Uint256;
        num->set_hb(0);
        num->set_lb(atoll(arg_str.c_str()));
        val->set_allocated_num(num);
        arg->set_allocated_val(val);
    } else if (arg_str.find("\"")){
        std::string literal_str = arg_str.substr(1, arg_str.size() - 1);
        val->set_str_literal(literal_str);
        arg->set_allocated_val(val);
    } else {
        val->set_var_name(arg_str);
        arg->set_allocated_val(val);
    }
}

void LineToStatement(std::string line, vyper::Statement *statement){
    if (line.find("=") != -1){
        // TODO impl line to assignment
    } else if (line.find("(") != 1){
        std::cout << "process call" << std::endl;
        char *ptr = (char *)line.c_str();
        size_t line_len = line.size();
        char *next_ptr = (char *)memmem(ptr, line_len, "(", 1);
        size_t name_len = next_ptr - ptr;
        char name[name_len + 1];
        memset(&name, 0, name_len + 1);
        memcpy(&name, ptr, name_len);
        std::string name_str(name);
        ptr = next_ptr + 1;

        auto it = func_table.find(name_str);
        auto func_id = std::distance(func_table.begin(), it);
        std::cout << "func_id: " << func_id << std::endl;
        switch(func_id){
            case Builtins::concat:
                std::cout << "parse concat" << std::endl;
                break;
            case Builtins::convert:
                break;
            default:
                std::cout << "process function call" << std::endl;
                auto func_call = new vyper::FunctionCall;
                func_call->set_function_name(name_str);

                next_ptr = (char *)memmem(ptr, line_len - name_len - 1, ")", 1);
                size_t args_len = next_ptr - ptr;
                char args[args_len + 1];
                memset(&args, 0, args_len + 1);
                memcpy(&args, ptr, args_len);
                std::string args_str(args);
                std::cout << "Enter ArgsSplit" << std::endl;
                auto arg_vec = ArgsSplit(args_str);

                for (int i = 0; i < arg_vec.size(); i++){
                    auto arg = func_call->add_args();
                    std::cout << "enter StrToValOrOp" << std::endl;
                    StrToValOrOp(arg_vec[i], arg);
                }
                statement->set_allocated_function_call(func_call);
        }
    } else if (line.rfind("return ", 0) == 0){
        // TODO impl return to line
    }
}

vyper::CodeBlock *LinesToBlock(std::vector<std::string> lines){
    auto code = new vyper::CodeBlock;
    
    for (int i = 0; i < lines.size(); i++){
        std::cout << "LinesToBlock loop" << std::endl;
        auto statement = code->add_statements();
        LineToStatement(lines[i], statement);
        std::cout << "passed LineToStatement" << std::endl;
    }

    return code;
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

        std::cout << "enter CodeBlockToStr" << std::endl;
        std::string block_str = CodeBlockToStr(block);

        contract = contract.append(fmt::format("def {}({}) -> {}:\n{}\n",
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

        char *next_ptr = (char *)memmem(local_ptr, left, "def ", 4);
        local_ptr = next_ptr + 4;

        // Parse function name
        next_ptr = (char *)memmem(local_ptr, left, "(", 1);
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

        // Parse return type
        next_ptr = (char *)memmem(local_ptr, left, ":\n", 2);
        size_t ret_len = next_ptr - local_ptr;
        char ret[ret_len + 1];
        memset(&ret, 0, ret_len + 1);
        memcpy(&ret, local_ptr, ret_len);
        local_ptr = next_ptr + 2;
        std::string ret_str(ret);
        auto ret_type = StrToType(ret_str);

        // Parse code block
        auto lines = GetBlockLines(&local_ptr, left);
        auto block = LinesToBlock(lines);

        // Assemble proto
        auto function = contract_proto->add_functions();
        function->set_external(external);
        function->set_function_name(name_str);
        function->set_allocated_return_type(ret_type);
        function->set_allocated_block(block);

        for (int i = 0; i < args.size(); i++){
            auto arg = function->add_args();
            arg->set_arg_name(args[i]->arg_name());
            arg->set_allocated_arg_type((vyper::Type *)&args[i]->arg_type());
        }

        external = false;
        left -= (local_ptr - ptr);
        ptr = local_ptr;
    }

    return contract_proto;
}

