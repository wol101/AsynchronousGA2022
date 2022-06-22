/*
 *  Genome.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <vector>
#include <limits>

class Random;

class Genome
{
public:

    Genome();
    Genome(const Genome &g);
    Genome(Genome &&g);
    Genome& operator=(const Genome &g);
    Genome& operator=(Genome &&g);

    enum GenomeType
    {
        IndividualRanges = -1,
        IndividualCircularMutation = -2
    };    

    double GetGene(size_t i) const { return mGenes[i]; }
    size_t GetGenomeLength() const { return mGenes.size(); }
    double GetHighBound(size_t i) const { return mHighBounds[i]; }
    double GetLowBound(size_t i) const { return mLowBounds[i]; }
    double GetGaussianSD(size_t i) const { return mGaussianSDs[i]; }
    double GetFitness() const { return mFitness; }
    GenomeType GetGenomeType() const { return mGenomeType; }
    std::vector<double> *GetGenes() { return &mGenes; }
    bool GetCircularMutation(int i);
    bool GetGlobalCircularMutationFlag() { return mGlobalCircularMutationFlag; }

    void Randomise(Random *random);
    void SetGene(size_t i, double value) { mGenes[i] = value; }
    void SetFitness(double fitness) { mFitness = fitness; }
    void SetCircularMutation(size_t i, bool circularFlag);
    void SetGlobalCircularMutationFlag(bool circularFlag) { mGlobalCircularMutationFlag = circularFlag; }
    void Clear();

    bool operator<(const Genome &in);

    friend std::ostream& operator<<(std::ostream &out, const Genome &g);
    friend std::istream& operator>>(std::istream &in, Genome &g);

private:

    std::vector<double> mGenes;
    std::vector<double> mLowBounds;
    std::vector<double> mHighBounds;
    std::vector<double> mGaussianSDs;
    std::vector<char> mCircularMutationFlags;
    GenomeType mGenomeType = IndividualRanges;
    bool mGlobalCircularMutationFlag = false;
    double mFitness = -std::numeric_limits<double>::max();
};

//bool GenomeFitnessLessThan(Genome *g1, Genome *g2);

#endif // GENOME_H
