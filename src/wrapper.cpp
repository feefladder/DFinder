#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ReferenceFinder_console.h"
// #include "ReferenceFinder.cpp"
// #include "ReferenceFinder.h"
// #include "FindDivisions.cpp"
// #include "FindDivisions.h"
// #include "parser.cpp"
// #include "lexer.cpp"
// #include "lexer.h"
#include "SVGDgmr.h"
#include "ReferenceFinder.h"
#include "FindDivisions.h"


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

class Pet {
public:
    Pet(){ }
    std::string name = "Mary";
};

PYBIND11_MODULE(dfinder, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: dfinder

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";
    m.def("InitializeSquare",  &InitializeSquare,  "Initialize a square of paper");
    m.def("CalcDivisionsHTML", &CalcDivisionsHTML, "Calculate divisions for a square");

    py::class_<instruction>(m, "instruction")
        .def(py::init())
        // .def_readonly("name", &Pet::name);
        .def_readwrite("description", &instruction::description)
        .def_readwrite("diagrams",    &instruction::diagrams)
        .def_readwrite("verbal",      &instruction::verbal);

    m.def("FindDivisionsSVG", &FindDivisionsSVG, "find folding sequences for all divisions");
    m.def("FoldCyclesSVG",    &FoldCyclesSVG,    "fold all remaining lines, starting from start");


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
