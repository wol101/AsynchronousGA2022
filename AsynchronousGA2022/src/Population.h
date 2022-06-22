/*
 *  Population.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef POPULATION_H
#define POPULATION_H

#include "Genome.h"

#include <deque>
#include <map>

class Random;

enum SelectionType
{
    UniformSelection,
    RankBasedSelection,
    SqrtBasedSelection,
    GammaBasedSelection
};

enum ResizeControl
{
    RandomiseResize,
    MutateResize
};

class Population
{
public:
    Population();

    void InitialisePopulation(size_t populationSize, const Genome &genome);

    Genome *GetFirstGenome() { return &m_Population.begin()->second; }
    Genome *GetLastGenome() { return &m_Population.rbegin()->second; }
    Genome *GetGenome(size_t i) { return &m_Population[m_PopulationIndex[i]]; }
    size_t GetPopulationSize() { return m_Population.size(); }

    void SetSelectionType(SelectionType type) { m_SelectionType = type; }
    void SetParentsToKeep(size_t parentsToKeep) { m_ParentsToKeep = parentsToKeep; if (m_ParentsToKeep < 0) m_ParentsToKeep = 0; }
    void SetGlobalCircularMutation(bool circularMutation);
    void SetResizeControl(ResizeControl control) { m_ResizeControl = control; }
    void SetGamma(double gamma) { m_Gamma = gamma; }

    Genome *ChooseParent(size_t *parentRank, Random *random);
    void Randomise(Random *random);
    int InsertGenome(Genome &&genome, size_t targetPopulationSize);
    void ResizePopulation(size_t size, Random *random);

    int ReadPopulation(const char *filename);
    int WritePopulation(const char *filename, size_t nBest);

protected:

    std::map<double, Genome> m_Population;
    std::vector<double> m_PopulationIndex; // sorted vector
    std::vector<double> m_ImmortalListIndex; // sorted vector
    std::deque<double> m_AgeList;
    SelectionType m_SelectionType = RankBasedSelection;
    size_t m_ParentsToKeep = 0;
    ResizeControl m_ResizeControl = MutateResize;
    double m_Gamma = 1.0;

};


#endif // POPULATION_H
