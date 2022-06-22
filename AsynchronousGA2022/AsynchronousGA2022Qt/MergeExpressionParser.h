// This class is used for parsing simple text math expressions

#ifndef MergeExpressionParser_h
#define MergeExpressionParser_h

#include <vector>
#include <string>
#include <map>

//#define exprtk_enable_debugging // this line turns on error reporting in the expression parser
#include "exprtk.hpp"

struct exprtk_parser;

struct MergeExpressionInput
{
    double v;
};

class MergeExpressionParser
{
public:
    MergeExpressionParser();
    ~MergeExpressionParser();

    int CreateFromString(char *input, int length);
    double Evaluate(MergeExpressionInput *expressionInput);

    void setExtraVariables(std::map<std::string, double> *extraVariables);

protected:
    exprtk_parser *m_exprtk_parser;
    double m_v;

    std::map<std::string, double> *m_extraVariables = nullptr;
};

#endif

