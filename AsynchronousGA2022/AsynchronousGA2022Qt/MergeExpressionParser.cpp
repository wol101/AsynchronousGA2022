// This class is used for parsing simple text math expressions

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "MergeExpressionParser.h"

extern int gMergeXMLVerboseLevel;

struct exprtk_parser
{
    exprtk::symbol_table<double> symbol_table;
    exprtk::expression<double>   expression;
    exprtk::parser<double>       parser;
};


MergeExpressionParser::MergeExpressionParser()
{
    m_exprtk_parser = new exprtk_parser();
}


MergeExpressionParser::~MergeExpressionParser()
{
    delete m_exprtk_parser;
}


// initialise the MergeExpressionParser based on a string
// returns 0 if successful
// the character input[length] must exist (can be '\0')
int MergeExpressionParser::CreateFromString(char *input, int length)
{
    bool success;
    std::string expression_string(input, length);

    m_exprtk_parser->symbol_table.add_variable("v", m_v);
    if (m_extraVariables)
    {
        for (auto &&it : *m_extraVariables)
        {
            m_exprtk_parser->symbol_table.add_variable(it.first, it.second);
        }
    }
    m_exprtk_parser->symbol_table.add_constants();

    m_exprtk_parser->expression.register_symbol_table(m_exprtk_parser->symbol_table);

    success = m_exprtk_parser->parser.compile(expression_string, m_exprtk_parser->expression);

    if (success == false) return __LINE__;
    return 0;
}

// Evaluate expression from expressionInput
double MergeExpressionParser::Evaluate(MergeExpressionInput *expressionInput)
{
    m_v = expressionInput->v;
    return m_exprtk_parser->expression.value();
}

void MergeExpressionParser::setExtraVariables(std::map<std::string, double> *extraVariables)
{
    m_extraVariables = extraVariables;
}


