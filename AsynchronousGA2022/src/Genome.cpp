/*
 *  Genome.cpp
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 19/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#include <assert.h>
#include <iostream>
#include <float.h>
#include <iomanip>
#include <ios>
#include <limits>

#include "Genome.h"
#include "Random.h"

// constructor
Genome::Genome()
{
}

Genome::Genome(const Genome &in)
{
    mGenes = in.mGenes;
    mLowBounds = in.mLowBounds;
    mHighBounds = in.mHighBounds;
    mGaussianSDs = in.mGaussianSDs;
    mCircularMutationFlags = in.mCircularMutationFlags;
    mGenomeType = in.mGenomeType;
    mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;
    mFitness = in.mFitness;
}

Genome::Genome(Genome &&in)
{
    mGenes = in.mGenes;
    mLowBounds = in.mLowBounds;
    mHighBounds = in.mHighBounds;
    mGaussianSDs = in.mGaussianSDs;
    mCircularMutationFlags = in.mCircularMutationFlags;
    mGenomeType = in.mGenomeType;
    mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;
    mFitness = in.mFitness;
}

// define = operators
Genome &Genome::operator=(const Genome &in)
{
    if (&in != this)
    {
        mGenes = in.mGenes;
        mLowBounds = in.mLowBounds;
        mHighBounds = in.mHighBounds;
        mGaussianSDs = in.mGaussianSDs;
        mCircularMutationFlags = in.mCircularMutationFlags;
        mGenomeType = in.mGenomeType;
        mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;
        mFitness = in.mFitness;
    }
    return *this;
}

Genome &Genome::operator=(Genome &&in)
{
    if (&in != this)
    {
        mGenes = in.mGenes;
        mLowBounds = in.mLowBounds;
        mHighBounds = in.mHighBounds;
        mGaussianSDs = in.mGaussianSDs;
        mCircularMutationFlags = in.mCircularMutationFlags;
        mGenomeType = in.mGenomeType;
        mGlobalCircularMutationFlag = in.mGlobalCircularMutationFlag;
        mFitness = in.mFitness;
    }
    return *this;
}

void Genome::Clear()
{
    mGenes.clear();
    mLowBounds.clear();
    mHighBounds.clear();
    mGaussianSDs.clear();
    mCircularMutationFlags.clear();
    mGenomeType = IndividualRanges;
    mGlobalCircularMutationFlag = false;
    mFitness = -std::numeric_limits<double>::max();
}

// randomise the genome
void Genome::Randomise(Random *random)
{
    switch (mGenomeType)
    {
    case IndividualRanges:
    case IndividualCircularMutation:
        for (size_t i = 0; i < mGenes.size(); i++)
        {
            if (mGaussianSDs[i] != 0)
                mGenes[i] = random->RandomDouble(mLowBounds[i], mHighBounds[i]);
        }
        break;
    }
}


// define < operator
bool Genome::operator<(const Genome &in)
{
    std::cerr << "Fitness test " << this->GetFitness() << " " << in.GetFitness() << "\n";
    if (this->GetFitness() < in.GetFitness()) return true;
    return false;
}

// get the value of the circular mutation flag for a gene
bool Genome::GetCircularMutation(int i)
{
    bool v = 0;

    switch (mGenomeType)
    {
    case IndividualRanges:
        v = mGlobalCircularMutationFlag;
        break;

    case IndividualCircularMutation:
        v = mCircularMutationFlags[i];
        break;
    }

    return v;
}

// set the value of the circular mutation flag for a gene
void Genome::SetCircularMutation(size_t i, bool circularFlag)
{
    switch (mGenomeType)
    {
    case IndividualRanges:
        mGlobalCircularMutationFlag = circularFlag;
        break;

    case IndividualCircularMutation:
        mCircularMutationFlags[i] = circularFlag;
        break;
    }
}


// output to a stream
std::ostream& operator<<(std::ostream &out, const Genome &g)
{
    switch (g.mGenomeType)
    {
    case Genome::IndividualRanges:
        out << g.mGenomeType << "\n";
        out << g.mGenes.size() << "\n";
        for (size_t i = 0; i < g.mGenes.size(); i++)
        {
            out << std::scientific << std::setprecision(18) << g.mGenes[i] << "\t" <<
                   std::defaultfloat << std::setprecision(8) << g.mLowBounds[i] << "\t" <<
                   g.mHighBounds[i] << "\t" << g.mGaussianSDs[i] << "\n";
        }
        out << std::scientific << std::setprecision(18) << g.mFitness << std::defaultfloat << "\t0\t0\t0\t0\n";
        break;

    case Genome::IndividualCircularMutation:
        out << g.mGenomeType << "\n";
        out << g.mGenes.size() << "\n";
        for (size_t i = 0; i < g.mGenes.size(); i++)
        {
            out << std::scientific << std::setprecision(18) << g.mGenes[i] << "\t" <<
                   std::defaultfloat << std::setprecision(8) << g.mLowBounds[i] << "\t" <<
                   g.mHighBounds[i] << "\t" << g.mGaussianSDs[i] << "\t" << g.mCircularMutationFlags[i] << "\n";
        }
        out << std::scientific << std::setprecision(18) << g.mFitness << std::defaultfloat << "\t0\t0\t0\t0\n";
        break;
    }

    return out;
}

// input from a stream
std::istream& operator>>(std::istream &in, Genome &g)
{
    int genomeType;
    size_t genomeLength;
    int dummy = 0;

    in >> genomeType;
    in >> genomeLength;

    g.Clear();
    g.mGenes.resize(genomeLength);
    g.mLowBounds.resize(genomeLength);
    g.mHighBounds.resize(genomeLength);
    g.mGaussianSDs.resize(genomeLength);
    g.mCircularMutationFlags.resize(genomeLength);

    g.mGenomeType = (Genome::GenomeType)genomeType;
    switch (g.mGenomeType)
    {
    case Genome::IndividualRanges:
        for (size_t i = 0; i < genomeLength; i++)
            in >> g.mGenes[i] >> g.mLowBounds[i] >> g.mHighBounds[i] >> g.mGaussianSDs[i];
        in >> g.mFitness >> dummy >> dummy >> dummy >> dummy;
        break;

    case Genome::IndividualCircularMutation:
        for (size_t i = 0; i < genomeLength; i++)
            in >> g.mGenes[i] >> g.mLowBounds[i] >> g.mHighBounds[i] >> g.mGaussianSDs[i] >> g.mCircularMutationFlags[i];
        in >> g.mFitness >> dummy >> dummy >> dummy >> dummy;
        break;

    }

    return in;
}

//bool GenomeFitnessLessThan(Genome *g1, Genome *g2)
//{
//    if (g1->GetFitness() < g2->GetFitness()) return true;
//    return false;
//}


