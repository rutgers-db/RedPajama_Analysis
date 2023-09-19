/**
 * NN Descent algorithm, Super Class
 * 
 * Author: Weiqi Feng
 * Date:   Jan 30, 2020
 * Email:  fengweiqi@sjtu.edu.cn
 */
#ifndef SUPERDESCENT_H
#define SUPERDESCENT_H

#include <cstdio>
#include <climits>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <algorithm>

using namespace std;

template <class T>
class SuperDescent
{
private:

    int MINSIZE;
    // Parameters
    double _rho;
    double _delta;
    int _maxIterations;
    int _k;

public:
    // Default constructor for NN-Descent
    SuperDescent() : MINSIZE(500), _rho(0.5), _delta(0.001), _maxIterations(INT_MAX), _k(10) {};

    // Initialize NN-Descent with given parameters
    SuperDescent(double rho, double delta, int maxIterations, int k) : MINSIZE(500), _rho(rho),
                                                                    _delta(delta), _maxIterations(maxIterations), _k(k) {};
    int getMinSize() const
    {
        return MINSIZE;
    }
    
    // Sample coefficient
    // @return the rho
    double getRho() const
    {
        return _rho;
    }

    // Early termination efficient
    // @return delata
    double getDelta() const
    {
        return _delta;
    }

    // Return the maximum number of iterations
    double getMaxIterations() const
    {
        return _maxIterations;
    }

    // Return the number of nearest neighbors
    int getK() const
    {
        return _k;
    }

    // Compute K-NN Graph
    virtual vector<vector<int>> computeGraph(const vector<T> &nodes, function<double(T, T)> fn) = 0;
};

#endif
