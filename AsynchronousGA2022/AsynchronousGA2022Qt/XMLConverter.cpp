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
#include "pystring.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
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
    const char *ptr2 = strstr(ptr1, m_startMarker.c_str());
    while (ptr2)
    {
        std::string s(ptr1, static_cast<size_t>(ptr2 - ptr1));
        m_smartSubstitutionTextComponents.push_back(std::move(s));

        ptr2 += 2;
        ptr1 = strstr(ptr2, m_endMarker.c_str());
        if (ptr1 == nullptr)
        {
            std::cerr << "Error: could not find matching " << m_endMarker << "\n";
            exit(1);
        }
        std::string expressionParserText = DecodeXMLExtities(std::string(ptr2, static_cast<size_t>(ptr1 - ptr2)));
        m_smartSubstitutionParserText.push_back(std::move(expressionParserText));
        m_smartSubstitutionValues.push_back(0); // dummy values
        ptr1 += 2;
        ptr2 = strstr(ptr1, m_startMarker.c_str());
    }
    std::string s(ptr1);
    m_smartSubstitutionTextComponents.push_back(std::move(s));

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

const std::string &XMLConverter::BaseXMLString() const
{
    return m_baseXMLString;
}

void XMLConverter::ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML, const std::string &startMarker, const std::string &endMarker)
{
    DataFile genomeData;
    double val;
    int ival, genomeSize;
    genomeData.ReadFile(inputGenome);
    genomeData.ReadNext(&ival);
    genomeData.ReadNext(&genomeSize);
    std::vector<double> data;
    data.reserve(size_t(genomeSize));
    for (int i = 0; i < genomeSize; i++)
    {
        genomeData.ReadNext(&val);
        data.push_back(val);
        genomeData.ReadNext(&val); genomeData.ReadNext(&val); genomeData.ReadNext(&val);
        if (ival == -2) genomeData.ReadNext(&val); // skip the extra parameter
    }

    XMLConverter myXMLConverter;
    myXMLConverter.SetStartEndMarkers(startMarker, endMarker);
    myXMLConverter.LoadBaseXMLFile(inputXML.c_str());
    myXMLConverter.ApplyGenome(genomeSize, data.data());
    std::string formatedXML;
    myXMLConverter.GetFormattedXML(&formatedXML);

    DataFile outputXMLData;
    outputXMLData.SetRawData(formatedXML.data(), formatedXML.size());
    outputXMLData.WriteFile(outputXML);
}

std::string XMLConverter::DecodeXMLExtities(const std::string &input)
{
    std::string result = input;
    static const std::map<std::string, std::string> decoder = { {"&amp;", "&"},  {"&gt;", ">"},  {"&lt;", "<"},  {"&quot;", "\""} };
    for (auto &&it : decoder)
    {
        result = pystring::replace(result, it.first, it.second);
    }
    return result;
}

void XMLConverter::SetStartEndMarkers(const std::string &startMarker, const std::string &endMarker)
{
    m_startMarker = startMarker;
    m_endMarker = endMarker;
}

