#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/pytypes.h>
#include "generate.h"

namespace py = pybind11;
constexpr auto byref = py::return_value_policy::reference_internal;

class VyperFuzzPy {
    private:
        VyperFuzz fuzz;
    
    public:
        py::bytes ProtoToVyper(const std::string contract_proto){
            // std::cout << "ProtoToVyper size:" << contract_proto.size() << std::endl;
            py::bytes res(fuzz.ProtoToVyper(contract_proto));
            return res;
        }

        py::bytes VyperToProto(const std::string contract){
            py::bytes res(fuzz.VyperToProto(contract));
            return res;
        }

        py::bytes Mutate(const std::string input, size_t max_size_hint){
            auto mutated = fuzz.Mutate(input, max_size_hint);
            // std::cout << mutated << std::endl;
            if (mutated.empty()){
                return input;
            }
            py::bytes res(mutated);
            // std::cout << "Passed py::bytes()" << std::endl;
            return res;
        }
};

PYBIND11_MODULE(VyperFuzzLib, m) {
    m.doc() = "Vyper Libprotobuf-mutator";

    py::class_<VyperFuzzPy>(m, "VyperFuzz")
    .def(py::init<>())
    .def("ProtoToVyper", &VyperFuzzPy::ProtoToVyper, py::call_guard<py::gil_scoped_release>())
    .def("VyperToProto", &VyperFuzzPy::VyperToProto, py::call_guard<py::gil_scoped_release>())
    .def("Mutate", &VyperFuzzPy::Mutate, py::call_guard<py::gil_scoped_release>());
}