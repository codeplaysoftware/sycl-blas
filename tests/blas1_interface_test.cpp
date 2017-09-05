/***************************************************************************
 *
 *  @license
 *  Copyright (C) 2016 Codeplay Software Limited
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  For your convenience, a copy of the License has been included in this
 *  repository.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  SYCL-BLAS: BLAS implementation using SYCL
 *
 *  @filename blas1_interface_test.cpp
 *
 **************************************************************************/

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <interface/blas1_interface_sycl.hpp>

using namespace cl::sycl;
using namespace blas;

#define DEF_SIZE_VECT 1200
#define ERROR_ALLOWED 1.0E-6
// #define SHOW_VALUES   1

int main(int argc, char *argv[]) {
  size_t sizeV, returnVal = 0;
  double res;

  if (argc == 1) {
    sizeV = DEF_SIZE_VECT;
  } else if (argc == 2) {
    sizeV = atoi(argv[1]);
  } else {
    std::cout << "ERROR!! --> Incorrect number of input parameters"
              << std::endl;
    returnVal = 1;
  }
  if (returnVal == 0) {
    // CREATING DATA
    std::vector<double> vX(sizeV);
    std::vector<double> vY(sizeV);
    std::vector<double> vZ(sizeV);
    std::vector<double> vR(1);
    std::vector<double> vS(1);
    std::vector<double> vT(1);
    std::vector<double> vU(1);
    std::vector<IndVal<double>> vImax(
        1, IndVal<double>(std::numeric_limits<int>::max(),
                          std::numeric_limits<double>::min()));
    std::vector<IndVal<double>> vImin(
        1, IndVal<double>(std::numeric_limits<int>::max(),
                          std::numeric_limits<double>::max()));

    size_t vSeed, gap;
    double minV, maxV;

    // INITIALIZING DATA
    vSeed = time(NULL) / 10 * 10;

    minV = -10.0;
    maxV = 10.0;
    gap = (size_t)(maxV - minV + 1);
    srand(vSeed);
    std::for_each(std::begin(vX), std::end(vX),
                  [&](double &elem) { elem = minV + (double)(rand() % gap); });

    minV = -30.0;
    maxV = 10.0;
    gap = (size_t)(maxV - minV + 1);
    std::for_each(std::begin(vY), std::end(vY),
                  [&](double &elem) { elem = minV + (double)(rand() % gap); });

    // COMPUTING THE RESULTS
    size_t i = 0, indMax = 0, indMin = 0;
    double sum = 0.0, alpha = 1.1, dot = 0.0;
    double nrmX = 0.0, nrmY = 0.0, max = 0.0, min = 1e9;
    double diff = 0.0;
    double _cos, _sin, giv = 0.0;
    std::for_each(std::begin(vZ), std::end(vZ), [&](double &elem) {
      elem = vY[i] + alpha * vX[i];
      sum += std::abs(elem);
      dot += (elem * vX[i]);
      nrmX += vX[i] * vX[i];
      nrmY += elem * elem;
      if (std::abs(elem) > std::abs(max)) {
        max = elem, indMax = i;
      }
      if (std::abs(elem) < std::abs(min)) {
        min = elem, indMin = i;
      }
      if (i == 0) {
        diff = elem - vX[i];
        double num1 = vX[0], num2 = elem;
        _rotg(num1, num2, _cos, _sin);
      }
      giv += ((vX[i] * _cos + elem * _sin) * (elem * _cos - vX[i] * _sin));
      if (i == 0) {
        diff = (elem * _cos - vX[i] * _sin) - ((vX[i] * _cos + elem * _sin));
      } else if ((i + 1) == sizeV) {
        diff += (elem * _cos - vX[i] * _sin) - ((vX[i] * _cos + elem * _sin));
      }
      i++;
    });
    nrmX = std::sqrt(nrmX);
    nrmY = std::sqrt(nrmY);

    // CREATING THE SYCL QUEUE AND EXECUTOR
    SYCLDevice dev;
    {
      // CREATION OF THE BUFFERS
      buffer<double, 1> bX(vX.data(), range<1>{vX.size()});
      buffer<double, 1> bY(vY.data(), range<1>{vY.size()});
      buffer<double, 1> bZ(vZ.data(), range<1>{vZ.size()});
      buffer<double, 1> bR(vR.data(), range<1>{vR.size()});
      buffer<double, 1> bS(vS.data(), range<1>{vS.size()});
      buffer<double, 1> bT(vT.data(), range<1>{vT.size()});
      buffer<double, 1> bU(vU.data(), range<1>{vU.size()});
      buffer<IndVal<double>, 1> bImax(vImax.data(), range<1>{vImax.size()});
      buffer<IndVal<double>, 1> bImin(vImin.data(), range<1>{vImin.size()});

      // EXECUTION OF THE ROUTINES
      blas::execute(dev, _axpy(bX.get_count(), alpha, bX, 0, 1, bY, 0, 1));
      blas::execute(dev, _asum(bY.get_count(), bY, 0, 1, bR));
      blas::execute(dev, _dot(bY.get_count(), bX, 0, 1, bY, 0, 1, bS));
      blas::execute(dev, _nrm2(bY.get_count(), bY, 0, 1, bT));
      blas::execute(dev, _iamax(bY.get_count(), bY, 0, 1, bImax));
      blas::execute(dev, _iamin(bY.get_count(), bY, 0, 1, bImin));
      blas::execute(dev, _rot(bY.get_count(), bX, 0, 1, bY, 0, 1, _cos, _sin));
      blas::execute(dev, _dot(bY.get_count(), bX, 0, 1, bY, 0, 1, bU));
      blas::execute(dev, _swap(bY.get_count(), bX, 0, 1, bY, 0, 1));
    }

    // ANALYSIS OF THE RESULTS
    res = vR[0];
#ifdef SHOW_VALUES
    std::cout << "VALUES!! --> res = " << res << " , sum = " << sum
              << " , err = " << sum - res << std::endl;
#endif  //  SHOW_VALUES
    if (std::abs((res - sum) / res) > ERROR_ALLOWED) {
      std::cout << "ERROR!! --> res = " << res << " , sum = " << sum
                << " , err = " << sum - res << std::endl;
      returnVal += 2;
    }

    res = vS[0];
#ifdef SHOW_VALUES
    std::cout << "VALUES!! --> res = " << res << " , dot = " << dot
              << " , err = " << dot - res << std::endl;
#endif  //  SHOW_VALUES
    if (std::abs((res - dot) / res) > ERROR_ALLOWED) {
      std::cout << "ERROR!! --> res = " << res << " , dot = " << dot
                << " , err = " << dot - res << std::endl;
      returnVal += 4;
    }

    res = vT[0];
#ifdef SHOW_VALUES
    std::cout << "VALUES!! --> res = " << res << " , nrmY = " << nrmY
              << " , err = " << nrmY - res << std::endl;
#endif  //  SHOW_VALUES
    if (std::abs((res - nrmY) / res) > ERROR_ALLOWED) {
      std::cout << "ERROR!! --> res = " << res << " , nrmY = " << nrmY
                << " , err = " << nrmY - res << std::endl;
      returnVal += 8;
    }

    /* IndVal<double> ind = vImax[0]; */
/* #ifdef SHOW_VALUES */
    /* std::cout << "VALUES!! --> resInd = " << ind.getInd() */
    /*           << ", resMax = " << ind.getVal() << " , ind = " << indMax */
    /*           << " , max = " << max << std::endl; */
/* #endif  //  SHOW_VALUES */
    /* if (ind.getInd() != indMax) { */
    /*   std::cout << "ERROR!! --> resInd = " << ind.getInd() */
    /*             << ", resMax = " << ind.getVal() << " , ind = " << indMax */
    /*             << " , max = " << max << std::endl; */
    /*   returnVal += 16; */
    /* } */

    /* ind = vImin[0]; */
/* #ifdef SHOW_VALUES */
    /* std::cout << "VALUES!! --> resInd = " << ind.getInd() */
    /*           << ", resmin = " << ind.getVal() << " , ind = " << indMin */
    /*           << " , min = " << min << std::endl; */
/* #endif  //  SHOW_VALUES */
    /* if (ind.getInd() != indMin) { */
    /*   std::cout << "ERROR!! --> resInd = " << ind.getInd() */
    /*             << ", resmin = " << ind.getVal() << " , ind = " << indMin */
    /*             << " , min = " << min << std::endl; */
    /*   returnVal += 16; */
    /* } */

    /* res = vU[0]; */
/* #ifdef SHOW_VALUES */
    /* std::cout << "VALUES!! --> res = " << res << " , giv = " << giv */
    /*           << " , err = " << giv - res << std::endl; */
/* #endif  //  SHOW_VALUES */
    /* if (std::abs((res - giv) / res) > ERROR_ALLOWED) { */
    /*   std::cout << "ERROR!! --> res = " << res << " , giv = " << giv */
    /*             << " , err = " << giv - res << std::endl; */
    /*   returnVal += 32; */
    /* } */

    res = (vX[0] - vY[0]) + (vX[sizeV - 1] - vY[sizeV - 1]);
#ifdef SHOW_VALUES
    std::cout << "VALUES!! --> res = " << res << " , diff = " << diff
              << " , err = " << diff - res << std::endl;
#endif  //  SHOW_VALUES
    if (std::abs((res - diff) / res) > ERROR_ALLOWED) {
      std::cout << "ERROR!! --> res = " << res << " , diff = " << diff
                << " , err = " << diff - res << std::endl;
      returnVal += 64;
    }
  }

  return returnVal;
}
