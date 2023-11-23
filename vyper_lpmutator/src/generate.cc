#include "generate.h"
#include <fmt/format.h>
#include <string>

// TODO:
// Split this into multiple files
// Reimplement memmem ptr stuff to a use a function instead of repetitive code patterns
// Maybe try to parse protobuf -> ast -> code instead of protobuf -> code (https://pybind11.readthedocs.io/en/latest/advanced/pycpp/object.html#accessing-python-libraries-from-c)

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

enum Builtins {
    concat,
    convert,
    none
};

static std::unordered_map<std::string,Builtins> const func_table = {
    {"concat",Builtins::concat},
    {"convert",Builtins::convert},
    {"", Builtins::none}
};

std::string ValueToStr(vyper::Value val){
    std::string val_str;

    if (val.has_num()){
        val_str = std::to_string(val.num().lb());
    } else if (!val.str_literal().empty()){
        val_str = fmt::format("\"{}\"", val.str_literal());
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
    } else if (val.has_builtin_call()){
        val_str = BuiltinCallToStr(val.builtin_call());
    } else if (val.has_func_call()){
        // std::cout << "val or op is func call" << std::endl;
        val_str = FunctionCallToStr(val.func_call());
    }

    return val_str;
}

std::string TypeToStr(vyper::Type type){
    auto desc = vyper::TYPE_ENUM_descriptor();
    auto val = desc->FindValueByNumber(type.typeval())->name();
    return val;
}

std::string SizedTypeToStr(vyper::SizedType stype){
    auto desc = vyper::SIZED_TYPE_ENUM_descriptor();
    auto val = desc->FindValueByNumber(stype.typeval())->name();
    auto size = stype.size();

    return fmt::format("{}[{}]", val, size);
}

std::string BuiltinCallToStr(vyper::BuiltinCall call){
    std::string call_str;

    return call_str;
}

std::string FunctionCallToStr(vyper::FunctionCall func_call){
    std::string func_call_str;

    auto func_args = func_call.args();
    std::string args_str;

    if(func_call.self()){
        func_call_str = func_call_str.append("self.");
    }

    for (int i = 0; i < func_args.size(); i++){
        auto arg_str = ValOrOpToStr(func_args[i]);
        // std::cout << arg_str << std::endl;
        args_str = args_str.append(arg_str);
        if (i != func_args.size() -1){
            args_str = args_str.append(",");
        }
    }
    func_call_str = func_call_str.append(fmt::format("{}({})", func_call.function_name(), args_str));

    return func_call_str;
}

std::string VarDeclToStr(vyper::VarDecl decl){
    std::string decl_str;
    std::string type_str;

    if (decl.has_type()){
        type_str = TypeToStr(decl.type());
    } else if (decl.has_stype()){
        type_str = SizedTypeToStr(decl.stype());
    }
    decl_str = decl_str.append(fmt::format("{}: {}", decl.var_name(), type_str));

    return decl_str;
}

std::string AssignmentToStr(vyper::Assignment assign){
    std::string assign_str;

    // std::cout << "AssignmentToStr" << std::endl;
    if (assign.has_decl()){
        assign_str = assign_str.append(VarDeclToStr(assign.decl()));
    } else if (assign.has_variable_name()){
        assign_str = assign_str.append(assign.variable_name());
    }

    auto val_str = ValOrOpToStr(assign.value());
    assign_str = assign_str.append(fmt::format(" = {}", val_str));

    // std::cout << assign_str << std::endl;
    return assign_str;
}

std::string StatementToLine(vyper::Statement statement){
    std::string statement_str;

    if (statement.has_assignment()){
        statement_str = AssignmentToStr(statement.assignment())+"\n";
    } else if (statement.has_builtin_call()){
        // TODO impl builtin call to line
    } else if (statement.has_function_call()){
        statement_str = FunctionCallToStr(statement.function_call())+"\n";
    } else if(statement.has_decl()){
        statement_str = VarDeclToStr(statement.decl())+"\n";
    } else if (statement.has_return_()){
        statement_str = fmt::format("return {}\n", ValOrOpToStr(statement.return_()));
    }

    return statement_str;
}

std::string CodeBlockToStr(vyper::CodeBlock block){
    std::string block_str;

    auto statements = block.statements();
    for (int i = 0; i < statements.size(); i++){
        //// std::cout << "Enter statement to line" << std::endl;
        block_str = block_str.append(fmt::format("    {}", StatementToLine(statements[i])));
    }

    return block_str;
}

vyper::Type *StrToType(std::string type_str){
    auto type = new vyper::Type;
    auto desc = vyper::TYPE_ENUM_descriptor();
    auto val = desc->FindValueByName(type_str)->number();
    type->set_typeval(vyper::TYPE_ENUM(val)); 
    return type;
}

vyper::SizedType *StrToSizedType(std::string type_str){
    auto stype = new vyper::SizedType;

    char *name_ptr = (char *)type_str.c_str();
    size_t type_size = type_str.size();
    char *size_ptr = (char *)memmem(name_ptr, type_size, "[", 1) + 1;
    if(!size_ptr){
        // std::cout << "not found" << std::endl;
    }

    // std::cout << "Parse Name" << std::endl;
    size_t name_len = size_ptr - name_ptr - 1;
    // std::cout << "stack alloc: " << name_len << std::endl;
    char name[name_len + 1];
    // std::cout << "memset" << std::endl;
    memset(&name, 0, name_len + 1);
    // std::cout << "memcpy" << std::endl;
    memcpy(&name, name_ptr, name_len);
    // std::cout << "string()" << std::endl;
    std::string name_str(name);

    // std::cout << "Parse Size" << std::endl;
    size_t size_len = type_size - name_len - 1;
    char size[size_len + 1];
    memset(&size, 0, size_len + 1);
    memcpy(&size, size_ptr, size_len);
    uint32_t size_uint = atoi(size);

    // std::cout << "Get Enum" << std::endl;
    auto desc = vyper::SIZED_TYPE_ENUM_descriptor();
    auto val = desc->FindValueByName(name_str)->number();

    // std::cout << "Set Vals" << std::endl;
    stype->set_typeval(vyper::SIZED_TYPE_ENUM(val));
    stype->set_size(size_uint);

    return stype;
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
    //// std::cout << "name_str: " << name_str << std::endl;

    size_t type_len = size - name_len;
    char type[type_len + 1];
    memset(&type, 0, type_len + 1);
    memcpy(&type, type_ptr, type_len);
    std::string type_str(type);
    //// std::cout << "type_str: " << type_str << std::endl;
    if(type_str.find("[") != -1){
        // std::cout << "Enter StrToSizedType: " << type_str << std::endl;
        auto stype_obj = StrToSizedType(type_str);
        // std::cout << "Passed StrToSizedType" << std::endl;
        arg->set_allocated_stype(stype_obj);
        // std::cout << "set allocated stype" << std::endl;
    } else {
        auto type_obj = StrToType(type_str);
        arg->set_allocated_type(type_obj);
    }
    arg->set_arg_name(name_str);

    //// std::cout << "return arg" << std::endl;
    return arg;
}

std::vector<vyper::ArgDef *> StrToArgDefs(std::string args_str){
    std::vector<vyper::ArgDef *> args;

    char *ptr = (char *)args_str.c_str();
    size_t left = args_str.size();
    size_t count = 0;
    //// std::cout << args_str << std::endl;
    while (left){
        //// std::cout << left << std::endl;
        char *next_ptr = (char *)memmem(ptr, left, ",", 1);
        size_t arg_size;
        if(next_ptr){
            arg_size = next_ptr - ptr;
        } else {
            //// std::cout << "last case" << std::endl;
            arg_size = left;
        }
        char arg[arg_size + 1];
        memset(&arg, 0, arg_size + 1);
        memcpy(&arg, ptr, arg_size);
        std::string arg_str(arg);
        //// std::cout << arg_str << std::endl;
        // std::cout << "enter StrToArgDef" << std::endl;
        args.push_back(StrToArgDef(arg_str));
        // std::cout << "passed StrToArgDef" << std::endl;
        ptr += arg_size + 1;
        left -= arg_size;
    }
    return args;
}

std::string ArgDefToStr(vyper::ArgDef arg){
    std::string type_str;
    if (arg.has_stype()){
        type_str = SizedTypeToStr(arg.stype());
    } else if (arg.has_type()){
        type_str = TypeToStr(arg.type());
    }
    return fmt::format("{}: {}", arg.arg_name(), type_str);
}

std::vector<std::string> GetBlockLines(char **ptr, size_t left){
    std::vector<std::string> lines;
    while (1){
        //// std::cout << "GetBlockLines loop" << std::endl;
        if(memcmp(*ptr, "    ", 4)){
            //// std::cout << "not in block" << std::endl;
            break;
        }
        char *start_ptr = *ptr + 4;
        char *end_ptr = (char *)memmem(start_ptr, left - 4, "\n", 1);
        size_t line_len = end_ptr - start_ptr;
        char line[line_len + 1];
        memset(&line, 0, line_len + 1);
        memcpy(&line, start_ptr, line_len);
        std::string line_str(line);
        //// std::cout << "line: " << line_str << std::endl;
        lines.push_back(line);
        *ptr += line_len + 5;
    };
    //// std::cout << "returning lines" << std::endl;
    return lines;
}

std::vector<std::string> ArgsSplit(std::string args){
    std::vector<std::string> arg_vec; 
    char *ptr = (char *)args.c_str();
    size_t left = args.size();

    while(left){
        //// std::cout << left << std::endl;
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
    } else if (!arg_str.find("\"")){
        std::string literal_str = arg_str.substr(1, arg_str.size() - 2);
        val->set_str_literal(literal_str);
        arg->set_allocated_val(val);
    } else if (arg_str.find("(") != -1){
        arg->set_allocated_func_call(StrToFunctionCall(arg_str));
    } else {
        val->set_var_name(arg_str);
        arg->set_allocated_val(val);
    }
}

vyper::Assignment *StrToAssignment(std::string assign_str){
    auto assign = new vyper::Assignment;

    char *left_ptr = (char *)assign_str.c_str();
    size_t size = assign_str.size();
    char *right_ptr = (char *)memmem(left_ptr, size, " = " , 3) + 3;

    size_t left_len = right_ptr - left_ptr - 3;
    char left[left_len + 1];
    memset(&left, 0, left_len + 1);
    memcpy(&left, left_ptr, left_len);
    std::string left_str(left);

    size_t right_len = size - left_len;
    char right[right_len + 1];
    memset(&right, 0, right_len + 1);
    memcpy(&right, right_ptr, right_len);
    std::string right_str(right);

    // std::cout << "left: " << left_str << std::endl;
    // std::cout << "right: " << right_str << std::endl;

    if (left_str.find(":") != -1){
        assign->set_allocated_decl(StrToVarDecl(left_str));
    } else {
        assign->set_variable_name(left_str);
    }

    auto value = new vyper::ValOrOp;
    StrToValOrOp(right_str, value);
    assign->set_allocated_value(value);

    return assign;
}

vyper::VarDecl *StrToVarDecl(std::string decl_str){
    auto decl = new vyper::VarDecl;
    // TODO impl str to decl

    char *name_ptr = (char *)decl_str.c_str();
    size_t size = decl_str.size();
    char *type_ptr = (char *)memmem(name_ptr, size, ": ", 2) + 2;

    size_t name_len = type_ptr - name_ptr - 2;
    char name[name_len + 1];
    memset(&name, 0, name_len + 1);
    memcpy(&name, name_ptr, name_len);
    std::string name_str(name);
    // std::cout << name_str << std::endl;  

    size_t type_len = size - name_len;
    char type[type_len + 1];
    memset(&type, 0, type_len + 1);
    memcpy(&type, type_ptr, type_len);
    std::string type_str(type);

    // std::cout << type_str << std::endl;

    decl->set_var_name(name_str);
    if (type_str.find("[") != -1){
        auto stype_obj = StrToSizedType(type_str);
        decl->set_allocated_stype(stype_obj);
    } else {
        auto type_obj = StrToType(type_str);
        decl->set_allocated_type(type_obj);
    }

    return decl;
}

vyper::FunctionCall *StrToFunctionCall(std::string func_call_str){
    char *ptr = (char *)func_call_str.c_str();
    bool self = false;

    if(!memcmp(ptr, "self.", 5)){
        self = true;
        ptr += 5;
    }

    size_t func_call_str_len = func_call_str.size();
    char *next_ptr = (char *)memmem(ptr, func_call_str_len, "(", 1);
    size_t name_len = next_ptr - ptr;
    char name[name_len + 1];
    memset(&name, 0, name_len + 1);
    memcpy(&name, ptr, name_len);
    std::string name_str(name);
    ptr = next_ptr + 1;

    auto func_call = new vyper::FunctionCall;
    func_call->set_function_name(name_str);

    next_ptr = (char *)memmem(ptr, func_call_str_len - name_len - 1, ")", 1);
    size_t args_len = next_ptr - ptr;
    char args[args_len + 1];
    memset(&args, 0, args_len + 1);
    memcpy(&args, ptr, args_len);
    std::string args_str(args);
    auto arg_vec = ArgsSplit(args_str);

    for (int i = 0; i < arg_vec.size(); i++){
        auto arg = func_call->add_args();
        StrToValOrOp(arg_vec[i], arg);
    }
    func_call->set_self(self);

    return func_call;
}

vyper::BuiltinCall *StrToBuiltinCall(std::string builtin_call_str, int func_id){
    auto builtin_call = new vyper::BuiltinCall;
    switch(func_id){
        case Builtins::concat:
            // std::cout << "parse concat" << std::endl;
            break;
        case Builtins::convert:
            // std::cout << "parse convert" << std::endl;
            break;
    }

    return builtin_call;
}

void StrToCall(std::string call_str, vyper::Statement *statement){
        // std::cout << "process call" << std::endl;
        char *ptr = (char *)call_str.c_str();
        size_t call_str_len = call_str.size();
        char *next_ptr = (char *)memmem(ptr, call_str_len, "(", 1);
        size_t name_len = next_ptr - ptr;
        char name[name_len + 1];
        memset(&name, 0, name_len + 1);
        memcpy(&name, ptr, name_len);
        std::string name_str(name);
        ptr = next_ptr + 1;

        // TODO needs refactoring (find is returning 2 for concat?)
        auto it = func_table.find(name_str);
        auto func_id = std::distance(func_table.begin(), it);
        // std::cout << "func_id: " << func_id << std::endl;
        // std::cout << "concat: " << Builtins::concat << std::endl;
        if (func_id != Builtins::none){
            // TODO impl str to builtin call
            return;
        }

        auto func_call = StrToFunctionCall(call_str);
        statement->set_allocated_function_call(func_call);
}

void LineToStatement(std::string line, vyper::Statement *statement){
    // std::cout << line << std::endl;
    // std::cout << (long)line.find("(") << std::endl;
    // std::cout << line.size() << std::endl;
    if (!line.find("return ")){
        // std::cout << "parse return" << std::endl;
        auto ret = new vyper::ValOrOp;
        char *ret_val = (char *)line.c_str() + 7;
        std::string ret_val_str(ret_val, line.size() - 7);
        // std::cout << "retval: " << ret_val_str << std::endl;
        StrToValOrOp(ret_val_str, ret);
        statement->set_allocated_return_(ret);
    } else if (line.find("=") != -1){
        // std::cout << "assignment" << std::endl;
        auto assign = StrToAssignment(line);
        statement->set_allocated_assignment(assign);

    } else if (line.find("(") != -1){
        StrToCall(line, statement);
    } else if (line.find(": ") != -1){
        auto decl = StrToVarDecl(line);
        statement->set_allocated_decl(decl);
    }
}

vyper::CodeBlock *LinesToBlock(std::vector<std::string> lines){
    auto code = new vyper::CodeBlock;
    
    for (int i = 0; i < lines.size(); i++){
        //// std::cout << "LinesToBlock loop" << std::endl;
        auto statement = code->add_statements();
        LineToStatement(lines[i], statement);
        //// std::cout << "passed LineToStatement" << std::endl;
    }

    return code;
}

std::string ProtoToVyperInternal(const vyper::VyperContract *contract_proto){
    std::string contract;
    auto functions = contract_proto->functions();
    for (int i = 0; i < functions.size(); i++){

        auto function = functions[i];
        if (function.external()){
            contract = contract.append("@external\n");
        } else {
            contract = contract.append("@internal\n");
        }

        auto args = function.args();
        std::string args_str;
        for (int i = 0; i < args.size(); i++){
            args_str = args_str.append(ArgDefToStr(args[i]));
            if (i != args.size() -1){
                args_str = args_str.append(",");
            }
        }

        std::string ret_str;
        if (function.has_return_type()){
            ret_str = TypeToStr(function.return_type());
        } else {
            ret_str = SizedTypeToStr(function.return_stype());
        }

        auto block = function.block();

        //// std::cout << "enter CodeBlockToStr" << std::endl;
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

vyper::VyperContract *VyperToProtoInternal(const std::string contract){
    auto contract_proto = new vyper::VyperContract;

    char *ptr = (char *)contract.c_str();
    size_t left = contract.size();

    while (left){
        bool external = true;
        char *local_ptr = ptr;

        if (memcmp(ptr, "@external\n", 10)){
            external = false;
        }
        local_ptr += 10;

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
        //// std::cout << "enter StrArgDefs" << std::endl;
        auto args = StrToArgDefs(args_str);
        // std::cout << "passed StrArgDefs" << std::endl;

        // Parse return type
        next_ptr = (char *)memmem(local_ptr, left, ":\n", 2);
        size_t ret_len = next_ptr - local_ptr;
        char ret[ret_len + 1];
        memset(&ret, 0, ret_len + 1);
        memcpy(&ret, local_ptr, ret_len);
        local_ptr = next_ptr + 2;
        std::string ret_str(ret);

        // Parse code block
        auto lines = GetBlockLines(&local_ptr, left);
        auto block = LinesToBlock(lines);

        // Assemble proto
        auto function = contract_proto->add_functions();
        function->set_external(external);
        function->set_function_name(name_str);

        if(ret_str.find("[") != -1){
            auto ret_type = StrToSizedType(ret_str);
            function->set_allocated_return_stype(ret_type);
        } else {
            auto ret_type = StrToType(ret_str);
            function->set_allocated_return_type(ret_type);
        }
        
        function->set_allocated_block(block);

        for (int i = 0; i < args.size(); i++){
            auto new_arg = function->add_args();
            auto arg = args[i];
            new_arg->set_arg_name(arg->arg_name());
            if (arg->has_type()){
                // std::cout << "set_allocated_type" << std::endl;
                new_arg->set_allocated_type((vyper::Type *)&arg->type());
            } else if (arg->has_stype()){
                // std::cout << "set_allocated_stype" << std::endl;
                new_arg->set_allocated_stype((vyper::SizedType *)&arg->stype());
            }
        }

        external = true;
        left -= (local_ptr - ptr);
        ptr = local_ptr;
    }

    return contract_proto;
}

std::string VyperFuzz::ProtoToVyper(std::string contract_proto){
    auto contract = new vyper::VyperContract;
    contract->ParseFromString(contract_proto);
    auto contract_vyper = ProtoToVyperInternal(contract);

    return contract_vyper;
}

std::string VyperFuzz::VyperToProto(std::string contract){
    std::string serialized;
    auto contract_proto = VyperToProtoInternal(contract);
    contract_proto->SerializeToString(&serialized);

    return serialized;
}

std::string VyperFuzz::Mutate(std::string input, size_t max_size_hint){
    std::string output;
    auto contract = new vyper::VyperContract;

    contract->ParseFromString(input);
    // VyperFuzz::mutator.Seed(666);
    VyperFuzz::mutator.Mutate(contract, max_size_hint);
    contract->SerializeToString(&output);

    delete contract;
    return output;
}