/*
 * Subgroup Discovery
 * Copyright (C) 2020  Jurian Baas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// [[Rcpp::depends(RcppParallel)]]

#include <vector>
#include <map>
#include <Rcpp.h>
#include <RcppParallel.h>
#include "prim.h"
#include "mapreduce.h"

using namespace RcppParallel;
using namespace Rcpp;
using namespace std;

IntegerVector sortIndex(const NumericMatrix::ConstColumn& col) {
  IntegerVector index(col.size());
  for (int i = 0 ; i != index.size() ; i++) {
    index[i] = i;
  }
  sort(index.begin(), index.end(),
       [&](const int& a, const int& b) {
         return (col[a] < col[b]);
       }
  );
  return index;
}

int countCategories(const NumericMatrix::ConstColumn& col) {
  return unique(col).length();
}

List findSubBoxes(
    const RMatrix<double>& M,
    const RVector<double>& y,
    const RVector<int>& colTypes,
    const map<int, int>& colCats,
    const map<int, IntegerVector>& colOrders,
    const double& alpha,
    const double& minSup) {

  bool* mask = new bool[M.nrow()]{};
  int masked = 0;

  bool subBoxFound;

  List peelSteps = List::create();

  do {

    ColWorker cw(
        M,
        y,
        colTypes,
        colCats,
        colOrders,
        alpha,
        minSup,
        masked,
        mask);

    parallelReduce(0, M.ncol(), cw);

    subBoxFound = cw.subBoxFound;

    if(subBoxFound) {
      SubBox bestSubBox = cw.bestSubBox;

      // Add the new subbox to the mask
      for(size_t i = 0; i < bestSubBox.remove.size(); i++) {
        mask[bestSubBox.remove[i]] = true;
      }

      masked += bestSubBox.remove.size();
      peelSteps.push_back(bestSubBox.toList());
    }

  } while (subBoxFound);

  IntegerVector finalBoxIndex;
  for(size_t i = 0; i < M.nrow(); i++) {
    if(mask[i]) continue;
    finalBoxIndex.push_back(i);
  }

  List peelResult = List::create();

  peelResult["peels"] = peelSteps;
  peelResult["index"] = finalBoxIndex;

  return peelResult;
}

List peelCpp (
    const NumericMatrix& M,
    const NumericVector& y,
    const IntegerVector& colTypes,
    const double& alpha,
    const double& minSup) {

  map<int, int> colCats;
  map<int, IntegerVector> colOrders;

  // Pre-process the data here,
  // for categorical columns, we need to know the number of categories
  // and for numerical data we store the permutation which arranges the
  // data into ascending order
  for(int i = 0; i < colTypes.length(); i++ ) {

    if(colTypes[i] == COL_NUMERIC) {

      // We pass a reference to the column here (no copy)
      colOrders[i] = sortIndex(M(_, i));

    } else if(colTypes[i] == COL_CATEGORICAL) {

      // We pass a reference to the column here (no copy)
      colCats[i] = countCategories(M(_, i));

    } else {
      stop("Column nr %i is not numeric or categorical", i);
    }
  }

  return findSubBoxes(
    RMatrix<double>(M),
    RVector<double>(y),
    RVector<int>(colTypes),
    colCats,
    colOrders,
    alpha,
    minSup
  );
}

List predictCpp (
    const List& peelSteps,
    const NumericMatrix& M,
    const NumericVector& y) {

  const int N = M.nrow();

  int masked = 0;
  bool* mask = new bool[M.nrow()]{};

  List validationSteps = List::create();

  for(List::const_iterator it = peelSteps.begin(); it != peelSteps.end(); ++it) {

    const List peel = *it;
    const int colId = peel["column"];
    const int type = peel["type"];
    const double value = peel["value"];

    double quality = 0;

    int k = 1;
    vector<int> remove;

    NumericMatrix::ConstColumn column = M( _, colId);

    if(type == BOX_NUM_LEFT) {

      for(int i = 0; i < column.size(); i++) {

        if(mask[i]) continue;

        if(column[i] < value) {
          remove.push_back(i);
        } else {
          // Cumulative moving avarage of rows we keep
          quality += (y[i] - quality) / k++;
        }

      }

    } else if (type == BOX_NUM_RIGHT) {

      for(int i = 0; i < column.size(); i++) {

        if(mask[i]) continue;

        if(column[i] > value) {
          remove.push_back(i);
        } else {
          // Cumulative moving avarage of rows we keep
          quality += (y[i] - quality) / k++;
        }

      }

    } else if (type == BOX_CATEGORY) {

      for(int i = 0; i < column.size(); i++) {

        if(mask[i]) continue;

        if(column[i] == value) {
          remove.push_back(i);
        } else {
          // Cumulative moving avarage of rows we keep
          quality += (y[i] - quality) / k++;
        }

      }

    }

    // Add the new subbox to the mask
    for(size_t i = 0; i < remove.size(); i++) {
      mask[remove[i]] = true;
    }
    masked += remove.size();

    const int keepCount = N - masked;
    const double support = keepCount / (double)N;

    validationSteps.push_back(
      List::create(
        _["column"] = colId,
        _["type"] = type,
        _["value"] = value,
        _["quality"] = quality,
        _["support"] = support
      )
    );
  }

  IntegerVector finalBoxIndex;
  for(int i = 0; i < N; i++) {
    if(mask[i]) continue;
    finalBoxIndex.push_back(i);
  }

  List validationResult = List::create();

  validationResult["peels"] = validationSteps;
  validationResult["index"] = finalBoxIndex;

  return validationResult;

}

List simplifyRules(
    const List& peelSteps,
    const IntegerVector& colTypes,
    const int& bestBoxIndex) {

  List compressedBoxes = List::create();

  for(int col = 0; col < colTypes.size(); col++) {

    const int colType = colTypes[col];
    bool colUsed = false;
    List bestBoxes = List::create();

    if(colType == COL_NUMERIC) {

      // Find boxes for this column, but don't consider boxes that are worse than the best box
      for(int boxIdx = 0; boxIdx <= bestBoxIndex; boxIdx++) {

        const List peel = peelSteps[boxIdx];
        const int _col = peel["column"];
        const int _boxType = peel["type"];
        const double _value = peel["value"];

        if(_col != col) continue;
        colUsed = true;

        if(_boxType == BOX_NUM_LEFT) {

          if(bestBoxes.containsElementNamed("left")) {
            const List curBest = bestBoxes["left"];
            const double value = curBest["value"];
            if(_value > value) {
              bestBoxes["left"] = peel;
            }
          } else {
            bestBoxes["left"] = peel;
          }

        } else if (_boxType == BOX_NUM_RIGHT) {

          if(bestBoxes.containsElementNamed("right")) {
            const List curBest = bestBoxes["right"];
            const double value = curBest["value"];
            if(_value < value) {
              bestBoxes["right"] = peel;
            }
          } else {
            bestBoxes["right"] = peel;
          }
        }
      }

    } else if(colType == COL_CATEGORICAL) {

      for(int boxIdx = 0; boxIdx <= bestBoxIndex; boxIdx++) {

        const List peel = peelSteps[boxIdx];
        const int _col = peel["column"];
        if(_col != col) continue;

        colUsed = true;
        bestBoxes.push_back(peel);
      }

    }

    if(colUsed) compressedBoxes.push_back(bestBoxes);
  }

  return compressedBoxes;
}




