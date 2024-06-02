/*
 *  XMLConverter.cc
 *  GA
 *
 *  Created by Bill Sellers on Fri Dec 12 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#include "XMLConverter.h"
#include "DataFile.h"

#include "exprtk.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <cfloat>
#include <cmath>
#include <assert.h>
#include <iostream>
#include <sstream>

XMLConverter::XMLConverter()
{
}

// load the base file for smart substitution file
int XMLConverter::LoadBaseXMLFile(const char *filename)
{
    DataFile smartSubstitutionBaseXMLFile;
    if (smartSubstitutionBaseXMLFile.ReadFile(filename)) return 1;
    LoadBaseXMLString(smartSubstitutionBaseXMLFile.GetRawData(), smartSubstitutionBaseXMLFile.GetSize());
    return 0;
}

void XMLConverter::Clear()
{
    m_smartSubstitutionTextComponents.clear();
    m_smartSubstitutionParserText.clear();
    m_smartSubstitutionValues.clear();
    m_baseXMLString.clear();
}

// load the base XML for smart substitution file
int XMLConverter::LoadBaseXMLString(const char *dataPtr, size_t length)
{
    m_smartSubstitutionTextComponents.clear();
    m_smartSubstitutionParserText.clear();
    m_smartSubstitutionValues.clear();
    m_baseXMLString = std::string(dataPtr, length);

    const char *ptr1 = dataPtr;
    const char *ptr2 = strstr(ptr1, "[[");
    while (ptr2)
    {
        std::string s(ptr1, static_cast<size_t>(ptr2 - ptr1));
        m_smartSubstitutionTextComponents.push_back(std::move(s));

        ptr2 += 2;
        ptr1 = strstr(ptr2, "]]");
        if (ptr1 == nullptr)
        {
            std::cerr << "Error: could not find matching ]]\n";
            exit(1);
        }
        std::string expressionParserText(ptr2, static_cast<size_t>(ptr1 - ptr2));
        m_smartSubstitutionParserText.push_back(std::move(expressionParserText));
        m_smartSubstitutionValues.push_back(0); // dummy values
        ptr1 += 2;
        ptr2 = strstr(ptr1, "[[");
    }
    std::string s(ptr1);
    m_smartSubstitutionTextComponents.push_back(std::move(s));


    // get the vector brackets in the right format for exprtk if necessary
    ConvertVectorBrackets();

    return 0;
}

void XMLConverter::GetFormattedXML(std::string *formattedXML)
{
    std::ostringstream ss;
    ss.precision(17);
    for (size_t i = 0; i < m_smartSubstitutionValues.size(); i++)
    {
        ss << m_smartSubstitutionTextComponents[i] << m_smartSubstitutionValues[i];
    }
    ss << m_smartSubstitutionTextComponents[m_smartSubstitutionValues.size()];
    *formattedXML = ss.str();
}

// this needs to be customised depending on how the genome interacts with
// the XML file specifying the simulation
int XMLConverter::ApplyGenome(int genomeSize, double *genomeData)
{
    exprtk::symbol_table<double> symbol_table;
    exprtk::expression<double> expression;
    exprtk::parser<double> parser;

    for (unsigned int i = 0; i < m_smartSubstitutionParserText.size(); i++)
    {
        // set up the genome as a function g(locus) and evaluate

        symbol_table.add_vector("g", genomeData, size_t(genomeSize));
        symbol_table.add_constants();

        expression.register_symbol_table(symbol_table);

//        std::cerr << "substitution text " << i << ": " << *m_smartSubstitutionParserText[i] << "\n";
        bool success = parser.compile(m_smartSubstitutionParserText[i], expression);

        if (success)
        {
            m_smartSubstitutionValues[i] = expression.value();
//            std::cerr << "substitution value " << i << " = " << m_SmartSubstitutionValues[i] << "\n";
        }
        else
        {
            std::cerr << "Error: XMLConverter::ApplyGenome m_SmartSubstitutionParserComponents[" << i << "] does not evaluate to a number\n";
            std::cerr << "Applying standard fix up and setting to zero\n";
            m_smartSubstitutionValues[i] = 0;
        }
    }

    return 0;
}

// exprtk requires [] around vector indices whereas my parser used ()
// this routine converts the brackets around the g vector
void XMLConverter::ConvertVectorBrackets()
{
    int pCount;
    unsigned int j;
    for (unsigned int i = 0; i < m_smartSubstitutionParserText.size(); i++)
    {
        std::string &s = m_smartSubstitutionParserText[i];
        j = 0;
        while (j < s.size())
        {
            if (s[j] == 'g')
            {
                j++;
                if (j >= s.size()) break;
                while (j < s.size())
                {
                    if (s[j] < 33) j++; // this just skips any whitespace
                    else break;
                }
                if (j >= s.size()) break;
                if (s[j] == '(')
                {
                    s[j] = '[';
                    pCount = 1;
                    while (j < s.size())
                    {
                        if (s[j] == '(') pCount++;
                        if (s[j] == ')') pCount--;
                        if (pCount <= 0)
                        {
                            s[j] = ']';
                            break;
                        }
                        j++;
                    }
                }
            }
            j++;
        }
    }
}

const std::string &XMLConverter::BaseXMLString() const
{
    return m_baseXMLString;
}


