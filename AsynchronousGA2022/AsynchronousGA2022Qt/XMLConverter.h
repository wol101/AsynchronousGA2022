/*
 *  XMLConverter.h
 *  GA
 *
 *  Created by Bill Sellers on Fri Dec 12 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#ifndef XMLConverter_h
#define XMLConverter_h

#include <vector>
#include <string>

class Genome;
class DataFile;
class ExpressionParser;

class XMLConverter
{
public:
    XMLConverter();

    int LoadBaseXMLFile(const char *filename);
    int LoadBaseXMLString(const char *dataPtr, size_t length);
    int ApplyGenome(int genomeSize, double *genomeData);
    void GetFormattedXML(std::string *formattedXML);
    void SetStartEndMarkers(const std::string &startMarker, const std::string &endMarker);

    const std::string &BaseXMLString() const;

    void Clear();

    static void ApplyGenome(const std::string &inputGenome, const std::string &inputXML, const std::string &outputXML);
    static std::string DecodeXMLExtities(const std::string &input);

private:

    std::string m_baseXMLString;
    std::vector<std::string> m_smartSubstitutionTextComponents;
    std::vector<std::string> m_smartSubstitutionParserText;
    std::vector<double> m_smartSubstitutionValues;
    std::string m_startMarker = {"[["};
    std::string m_endMarker = {"]]"};
};


#endif
