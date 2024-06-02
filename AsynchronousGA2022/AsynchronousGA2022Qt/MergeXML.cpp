/*
 *  MergeXML.cc
 *  MergeXML
 *
 *  Created by Bill Sellers on Wed Dec 17 2003.
 *  Copyright (c) 2003 Bill Sellers. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "XMLContainer.h"
#include "XMLContainerList.h"
#include "DataFile.h"
#include "MergeUtil.h"

#include "MergeXML.h"

MergeXML::MergeXML()
{
}

// read and execute the instruction file
void MergeXML::ExecuteInstructionFile(const char *bigString)
{
    DataFile theFile;
    char buffer[32768];
    char line[32768];
    char *tokens[32768];
    int numTokens;
    int count = 0;
    int rCount;

    theFile.SetExitOnError(true);
    theFile.SetRawData(bigString, strlen(bigString));

    // do the global substitution
    if (m_oldStringList.size() > 0)
    {
        for (unsigned int i = 0; i < m_oldStringList.size(); i++)
        {
            rCount = int(theFile.Replace(m_oldStringList[i].c_str(), m_newStringList[i].c_str()));
            if (m_mergeXMLVerbosityLevel) std::cerr << "Replace \"" << m_oldStringList[i] <<
                "\" with \"" << m_newStringList[i] << "\" Count = " << rCount << "\n";
        }
    }

    while (theFile.ReadNextLine(line, sizeof(buffer), true, '#', '\\') == false)
    {
        count++;
        strncpy(buffer, line, sizeof(buffer));
        numTokens = int(DataFile::ReturnTokens(buffer, tokens, sizeof(tokens)));

        if (m_mergeXMLVerbosityLevel > 1)
            std::cerr << "ExecuteInstructionFile:\tnumTokens\t" << numTokens << "\n";

        if (m_mergeXMLVerbosityLevel) std::cerr << line << "\n";

        // read in a new XML file
        // ReadXMLFile XMLObjectName Filename
        if (strcmp(tokens[0], "ReadXMLFile") == 0)
        {
            if (numTokens != 3) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            if (m_objectList.Get(tokens[1])) { ParseError(tokens, numTokens, count, "XMLObject name not unique"); continue; }
            std::unique_ptr<XMLContainer> object = std::make_unique<XMLContainer>();
            object->SetName(tokens[1]);
            if (object->LoadXML(tokens[2]))
            { ParseError(tokens, numTokens, count, "Error reading XML file"); continue; }
            m_objectList.Add(std::move(object));
            continue;
        }

        // write a FacetedObject to a .obj file
        // WriteXMLFile XMLObjectName Filename
        if (strcmp(tokens[0], "WriteXMLFile") == 0)
        {
            if (numTokens != 3) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObject name not found"); continue; }
            if (object->WriteXML(tokens[2]))
            { ParseError(tokens, numTokens, count, "Error writing XML file"); continue; }
            continue;
        }

        // merge two XML files
        // Merge XMLObjectName1 XMLObjectName2 Element IDAttribute MergeAttribute Fraction
        // Merge XMLObjectName1 XMLObjectName2 Element IDAttribute MergeAttribute Fraction StartIndex EndIndex
        if (strcmp(tokens[0], "Merge") == 0)
        {
            if (numTokens != 7 && numTokens != 9) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            XMLContainer *object2 = m_objectList.Get(tokens[2]);
            if (object2 == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (numTokens == 7)
            {
                if (object->Merge(object2, tokens[3], tokens[4], tokens[5], MergeUtil::Double(tokens[6])))
                { ParseError(tokens, numTokens, count, "Error merging XML files"); continue; }
            }
            else if (numTokens == 9)
            {
                if (object->Merge(object2, tokens[3], tokens[4], tokens[5], MergeUtil::Double(tokens[6]), MergeUtil::Int(tokens[7]), MergeUtil::Int(tokens[8])))
                { ParseError(tokens, numTokens, count, "Error merging XML files"); continue; }
            }
            continue;
        }

        // merge two XML files only specified IDAttribute
        // MergeID XMLObjectName1 XMLObjectName2 Element IDAttribute ID MergeAttribute Fraction
        // MergeID XMLObjectName1 XMLObjectName2 Element IDAttribute ID MergeAttribute Fraction StartIndex EndIndex
        if (strcmp(tokens[0], "MergeID") == 0)
        {
            if (numTokens != 8 && numTokens != 10) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            XMLContainer *object2 = m_objectList.Get(tokens[2]);
            if (object2 == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (numTokens == 8)
            {
                if (object->MergeID(object2, tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Double(tokens[7])))
                { ParseError(tokens, numTokens, count, "Error merging XML files"); continue; }
            }
            else if (numTokens == 10)
            {
                if (object->MergeID(object2, tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Double(tokens[7]), MergeUtil::Int(tokens[8]), MergeUtil::Int(tokens[9])))
                { ParseError(tokens, numTokens, count, "Error merging XML files"); continue; }
            }
            continue;
        }

        // Operate on an XML value
        // Operate XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute Offset
        if (strcmp(tokens[0], "Operate") == 0)
        {
            if (numTokens != 8) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Operate(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Int(tokens[7]), &m_globalVariablesList))
            { ParseError(tokens, numTokens, count, "Error performing Operate"); continue; }
            continue;
        }

        // Set value of a Global
        // Global XMLObjectName1 GlobalName Element IDAttribute IDValue ChangeAttribute Offset
        if (strcmp(tokens[0], "Global") == 0)
        {
            if (numTokens != 8) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Global(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Int(tokens[7]), &m_globalVariablesList))
            { ParseError(tokens, numTokens, count, "Error performing Operate"); continue; }
            continue;
        }

        // Set an XML value (will work with string values and can also increase the length of the argument list by 1)
        // If Offset < 0 then the whole of the attribute is set to the new value
        // Set XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute Offset
        if (strcmp(tokens[0], "Set") == 0)
        {
            if (numTokens != 8) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Set(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], MergeUtil::Int(tokens[7])))
            { ParseError(tokens, numTokens, count, "Error performing Set"); continue; }
            continue;
        }

        // Scale an XML position key
        // Scale XMLObjectName1 Operation Element IDAttribute IDValue ChangeAttribute ReferenceID
        if (strcmp(tokens[0], "Scale") == 0)
        {
            if (numTokens != 8) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Scale(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]))
            { ParseError(tokens, numTokens, count, "Error performing Scale"); continue; }
            continue;
        }

        // Swap two XML values
        // Swap XMLObjectName1 Element IDAttribute IDValue ChangeAttribute Offset1 Offset2
        if (strcmp(tokens[0], "Swap") == 0)
        {
            if (numTokens != 8) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Swap(tokens[2], tokens[3], tokens[4], tokens[5], MergeUtil::Int(tokens[6]), MergeUtil::Int(tokens[7])))
            { ParseError(tokens, numTokens, count, "Error performing Swap"); continue; }
            continue;
        }

        // Generate a new XML values
        // Generate XMLObjectName1 Operation Element IDAttribute IDValue GenerateAttribute
        if (strcmp(tokens[0], "Generate") == 0)
        {
            if (numTokens != 7) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Generate(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]))
            { ParseError(tokens, numTokens, count, "Error performing Generate"); continue; }
            continue;
        }

        // Delete an XML values
        // Delete XMLObjectName1 Element IDAttribute IDValue DeleteAttribute

        if (strcmp(tokens[0], "Delete") == 0)
        {
            if (numTokens != 6) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Delete(tokens[2], tokens[3], tokens[4], tokens[5]))
            { ParseError(tokens, numTokens, count, "Error performing Delete"); continue; }
            continue;
        }

        // Create a new XML tag
        // Create XMLObjectName1 Element IDAttribute IDValue

        if (strcmp(tokens[0], "Create") == 0)
        {
            if (numTokens != 5) { ParseError(tokens, numTokens, count, "Wrong number of tokens"); continue; }
            XMLContainer *object = m_objectList.Get(tokens[1]);
            if (object == nullptr) { ParseError(tokens, numTokens, count, "XMLObjectName name not found"); continue; }
            if (object->Create(tokens[2], tokens[3], tokens[4]))
            { ParseError(tokens, numTokens, count, "Error performing Create"); continue; }
            continue;
        }

        ParseError(tokens, numTokens, count, "Unrecognised line");
    }
}

// Produce a parse error message
void MergeXML::ParseError(char **tokens, int numTokens, int count, const char *message)
{
//    std::cerr << "Parse error on line " << count << "\n";
//    for (int i = 0; i < numTokens; i++)
//        std::cerr << tokens[i] << " ";
//    std::cerr << "\n";
//    if (message) std::cerr << message << "\n";
    m_errorMessageList[count] = message;
    std::vector<std::string> errorTokens;
    for (int i = 0; i < numTokens; i++) errorTokens.push_back(tokens[i]);
    m_errorList[count] = std::move(errorTokens);
}

const std::map<int, std::string> &MergeXML::errorMessageList() const
{
    return m_errorMessageList;
}

const std::map<int, std::vector<std::string> > &MergeXML::errorList() const
{
    return m_errorList;
}



