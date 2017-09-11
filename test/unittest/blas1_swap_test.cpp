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
 *  @filename blas1_swap_test.cpp
 *
 **************************************************************************/

#include "blas1_test.hpp"

typedef ::testing::Types<blas1_test_args<float>, blas1_test_args<double> >
    BlasTypes;

TYPED_TEST_CASE(BLAS1_Test, BlasTypes);

REGISTER_SIZE(::RANDOM_SIZE, swap_test)
REGISTER_STRD(::RANDOM_STRD, swap_test)

TYPED_TEST(BLAS1_Test, swap_test) {
  using ScalarT = typename TypeParam::scalar_t;
  using ExecutorType = typename TypeParam::executor_t;
  using TestClass = BLAS1_Test<TypeParam>;
  using test = class swap_test;

  size_t size = TestClass::template test_size<test>();
  size_t strd = TestClass::template test_strd<test>();

  DEBUG_PRINT(std::cout << "size == " << size << std::endl);
  DEBUG_PRINT(std::cout << "strd == " << strd << std::endl);

  // create two random vectors with the same size: vX and vY
  std::vector<ScalarT> vX(size);
  std::vector<ScalarT> vY(size);
  TestClass::set_rand(vX, size);
  TestClass::set_rand(vY, size);

  // create two more vectors equal to vX and vY
  std::vector<ScalarT> vZ = vX;
  std::vector<ScalarT> vT = vY;

  SYCL_DEVICE_SELECTOR d;
  auto q = TestClass::make_queue(d);
  Executor<ExecutorType> ex(q);
  {
    // swap the elements between vX and vY with syclblas
    auto buf_vX = TestClass::make_buffer(vX);
    auto buf_vY = TestClass::make_buffer(vY);
    auto view_vX = TestClass::make_vview(buf_vX);
    auto view_vY = TestClass::make_vview(buf_vY);
    _swap(ex, (size+strd-1)/strd, view_vX, strd, view_vY, strd);
  }
  // check that new vX is equal to the copy of the original vY and
  // that new vY is equal to the copy of the original vX
  for (size_t i = 0; i < size; ++i) {
    if (i % strd == 0) {
      ASSERT_EQ(vZ[i], vY[i]);
      ASSERT_EQ(vT[i], vX[i]);
    } else {
      ASSERT_EQ(vZ[i], vX[i]);
      ASSERT_EQ(vT[i], vY[i]);
    }
  }
}
