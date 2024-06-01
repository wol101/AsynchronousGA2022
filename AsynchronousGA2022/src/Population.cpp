/*
 *  Population.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include <assert.h>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <float.h>
#include <map>
#include <list>
#include <cmath>
#include <limits>
#include <algorithm>

#include "Genome.h"
#include "Population.h"
#include "Random.h"
#include "Preferences.h"

bool GenomeFitnessLessThan(Genome *g1, Genome *g2);

// constructor
Population::Population()
{
}


// initialise the population
void Population::InitialisePopulation(size_t populationSize, const Genome &genome)
{
    m_Population.clear();
    m_PopulationIndex.resize(populationSize);
    for (size_t i = 0; i < populationSize; i++)
    {
        m_Population[double(i)] = genome;
        m_PopulationIndex[i] = double(i);
    }
}

// choose a parent from a population
Genome *Population::ChooseParent(size_t *parentRank, Random *random)
{
    // this type biases random choice to higher ranked individuals using the gamma function
    // this assumes a sorted genome
    if (m_SelectionType == GammaBasedSelection)
    {
        *parentRank = random->GammaBiasedRandomInt(0, m_Population.size() - 1, m_Gamma);
        return &m_Population[m_PopulationIndex[*parentRank]];
    }

    // in this version we do uniform selection and just choose a parent
    // at random
    if (m_SelectionType == UniformSelection)
    {
        *parentRank = random->RandomInt(0, m_Population.size() - 1);
        return &m_Population[m_PopulationIndex[*parentRank]];
    }

    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == RankBasedSelection)
    {
        *parentRank = random->RankBiasedRandomInt(1, m_Population.size()) - 1;
        return &m_Population[m_PopulationIndex[*parentRank]];
    }

    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == SqrtBasedSelection)
    {
        *parentRank = random->SqrtBiasedRandomInt(0, m_Population.size() - 1);
        return &m_Population[m_PopulationIndex[*parentRank]];
    }

    // should never get here
    std::cerr << "Logic error Population::ChooseParent " << __LINE__ << "\n";
    return 0;
}

// insert a genome into the population
// the population is always kept sorted by fitness
// the oldest genome is deleted unless it is in the
// immortal list
// the key is the numeric value of the fitness so there are rare cases when
// different genomes with the same fitness will not be accepted
int Population::InsertGenome(Genome &&genome, size_t targetPopulationSize)
{
    size_t originalSize = m_Population.size();
    if (targetPopulationSize == 0) targetPopulationSize = originalSize; // not trying to change the size of the population

    double fitness = genome.GetFitness();
    if (m_MinimizeScore) { fitness = -fitness; } // now all the sorting will hapen in reverse
    auto findGenome = m_Population.find(fitness);
    if (findGenome != m_Population.end())
    {
        // std::cerr << "InsertGenome fitness = " << fitness << " already in population\n";
        return __LINE__; // nothing inserted because the fitness already exists
    }
    m_Population[fitness] = genome;

    // ok now insert into the other internal lists
    m_PopulationIndex.insert(std::upper_bound(m_PopulationIndex.begin(), m_PopulationIndex.end(), fitness), fitness); // insert at upper bound to minimise shifting

    if (m_ParentsToKeep == 0)
    {
        m_AgeList.push_back(fitness); // just add current genome to list by age
        // std::cerr << "InsertGenome adding to m_AgeList - no test; size = " << m_AgeList.size() << "\n";
    }
    else
    {
        // need to worry about immortality
        if (m_ImmortalListIndex.size() < m_ParentsToKeep)
        {
            m_ImmortalListIndex.insert(std::upper_bound(m_ImmortalListIndex.begin(), m_ImmortalListIndex.end(), fitness), fitness);
            // std::cerr << "InsertGenome adding to m_ImmortalList - no test; size = " << m_ImmortalList.size() << "\n";
        }
        else
        {
            if (fitness > m_ImmortalListIndex[0])
            {
                m_ImmortalListIndex.insert(std::upper_bound(m_ImmortalListIndex.begin(), m_ImmortalListIndex.end(), fitness), fitness);
                m_AgeList.push_back(m_ImmortalListIndex.front());
                m_ImmortalListIndex.erase(m_ImmortalListIndex.begin());
                // std::cerr << "InsertGenome m_ImmortalList to m_AgeList bump; m_AgeList.size() = " << m_AgeList.size() << " m_ImmortalList.size() = " << m_ImmortalList.size() << "\n";
            }
            else
            {
                m_AgeList.push_back(fitness);
                // std::cerr << "InsertGenome adding to m_AgeList after test; size = " << m_AgeList.size() << "\n";
            }
        }
    }

    // check population sizes
    while (m_Population.size() > targetPopulationSize)
    {
        if (m_AgeList.size() == 0)
        {
            std::cerr << "Logic error Population::InsertGenome m_ageList has zero size " << __LINE__ << "\n";
            m_Population.erase(*m_PopulationIndex.begin());
            m_PopulationIndex.erase(m_PopulationIndex.begin());
            continue;
        }
        double genomeToDelete = *m_AgeList.begin();
        m_AgeList.pop_front();
        auto genomeIt = m_Population.find(genomeToDelete);
        if (genomeIt != m_Population.end())
        {
            m_Population.erase(genomeIt);
        }
        else
        {
            std::cerr << "Logic error Population::InsertGenome genome not found in m_Population " << __LINE__ << "\n";
            m_Population.erase(m_Population.begin());
            m_PopulationIndex.erase(m_PopulationIndex.begin());
            continue;
        }
        auto fitnessIt = std::lower_bound(m_PopulationIndex.begin(), m_PopulationIndex.end(), genomeToDelete); // delete at lower bound because this value matches (whereas upper bound does not)
        if (fitnessIt != m_PopulationIndex.end() && *fitnessIt == genomeToDelete)
        {
            m_PopulationIndex.erase(fitnessIt);
        }
        else
        {
            std::cerr << "Logic error Population::InsertGenome genome not found in m_PopulationIndex " << __LINE__ << "\n";
            m_PopulationIndex.clear(); // need to rebuild the index by getting all the indexes and sorting (quicker than inserting at a sorted location each time)
            for (auto &&it : m_Population) { m_PopulationIndex.push_back(it.first); }
            std::sort(m_PopulationIndex.begin(), m_PopulationIndex.end());
        }
    }
    return 0;
}

// randomise the population
void Population::Randomise(Random *random)
{
    for (auto iter = m_Population.begin(); iter != m_Population.end(); iter++)
        iter->second.Randomise(random);
}

// reset the population size to a new value - needs at least one valid genome in population
void Population::ResizePopulation(size_t size, Random *random)
{
    if (m_Population.size() == size) return;
    if (size > m_Population.size())
    {
        switch (m_ResizeControl)
        {
        case RandomiseResize:
            // fill in with random genomes
            for (size_t i = m_Population.size(); i < size; i++)
            {
                Genome g = m_Population.begin()->second;
                g.Randomise(random);
                g.SetFitness(std::nextafter(m_PopulationIndex.back(), std::numeric_limits<double>::max()));
                m_PopulationIndex.push_back(g.GetFitness());
                m_Population[g.GetFitness()] = std::move(g);
            }
            break;

        case MutateResize:
            // fill in with mutated genomes
            for (size_t i = m_Population.size(); i < size; i++)
            {
                Genome g = m_Population.begin()->second;
                Mating mating(random);
                while (mating.GaussianMutate(&g, 1.0, true) == 0);
                g.SetFitness(std::nextafter(m_PopulationIndex.back(), std::numeric_limits<double>::max()));
                m_PopulationIndex.push_back(g.GetFitness());
                m_Population[g.GetFitness()] = std::move(g);
            }
            break;

        }
    }
    else
    {
        size_t delta = m_Population.size() - size;
        for (size_t i = 0; i < delta; i++)
        {
            double fitness = m_PopulationIndex.front();
            m_PopulationIndex.erase(m_PopulationIndex.begin());
            m_Population.erase(fitness);
        }
    }
}

// set the circular flags for the genomes in the population
void Population::SetGlobalCircularMutation(bool circularMutation)
{
    for (auto iter = m_Population.begin(); iter != m_Population.end(); ++iter)
        iter->second.SetGlobalCircularMutationFlag(circularMutation);
}

// output a subpopulation as a new population
// note: outputs population with fittest first
int Population::WritePopulation(const char *filename, size_t nBest)
{
    if (nBest > m_Population.size()) nBest = m_Population.size();

    try
    {
        std::ofstream outFile;
        outFile.exceptions (std::ios::failbit|std::ios::badbit);
        outFile.open(filename);
        outFile << nBest << "\n";
        size_t count = 0;
        for (auto iter = m_Population.rbegin(); iter != m_Population.rend(); ++iter)
        {
            outFile << iter->second;
            count++;
            if (count >= nBest) break;
        }
        outFile.close();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
    catch (...)
    {
        return __LINE__;
    }
    return 0;
}

// read a population
// this can be quite slow because it re-sorts everything which may not be necessary
// and it may not preserve existing fitnesses if they are not all unique
int Population::ReadPopulation(const char *filename)
{
    std::ifstream inFile;
    inFile.exceptions (std::ios::failbit|std::ios::badbit|std::ios::eofbit);
    try
    {
        inFile.open(filename);

        m_Population.clear();
        m_PopulationIndex.clear();
        m_ImmortalListIndex.clear();
        m_AgeList.clear();

        size_t populationSize;
        inFile >> populationSize;
        bool warningEmitted = false;
        for (size_t i = 0; i < populationSize; i++)
        {
            Genome genome;
            inFile >> genome;
            // std::cerr << "Fitness = " << genome.GetFitness() << "\n";
            if (i > 0 && m_Population.find(genome.GetFitness()) != m_Population.end())
            {
                genome.SetFitness(std::nextafter(m_PopulationIndex.back(), std::numeric_limits<double>::max())); // this line forces all the genomes to be inserted since they might not have valid fitnesses
                if (!warningEmitted)
                {
                    std::cerr << "Warning: population contains duplicate fitness values. Setting to fake values. index first detected = " << i << "\n";
                    warningEmitted = true;
                }
            }
            InsertGenome(std::move(genome), populationSize);
        }
        inFile.close();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
    catch (...)
    {
        return __LINE__;
    }
    return 0;
}

