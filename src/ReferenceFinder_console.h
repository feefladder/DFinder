#include "SVGDgmr.h"
#include <string>

void InitializeSquare();
std::string CalcDivisionsHTML(int total);
std::vector<instruction> FindDivisionsSVG(int total);
instruction FoldCyclesSVG(int total, int start);