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
 *  @filename blas_packet_traits_sycl.hpp
 *
 **************************************************************************/

#ifndef BLAS_PACKET_TRAITS_SYCL_HPP
#define BLAS_PACKET_TRAITS_SYCL_HPP

#include <climits>

#include <CL/sycl.hpp>
#include <executors/blas_packet_traits.hpp>

namespace blas {

class SYCLDevice {
  /*!
   * @brief SYCL queue for execution of trees.
   */
  cl::sycl::queue m_queue;

 public:
  SYCLDevice():
    m_queue(cl::sycl::queue([&](cl::sycl::exception_list l) {
      for(const auto &e : l) {
        try {
          if(e) {
            std::rethrow_exception(e);
          }
        } catch(cl::sycl::exception e) {
          std::cerr << e.what() << std::endl;
        }
      }
    }))
  {}

  static void parallel_for_setup(size_t &localSize, size_t &nWG, size_t &globalSize, size_t N) {
    localSize = 256;
    globalSize = N;
    nWG = (globalSize + localSize - 1) / localSize;
  }

  cl::sycl::queue sycl_queue() { return m_queue; }
  cl::sycl::device sycl_device() { return m_queue.get_device(); }
};

template <typename T>
struct Packet_traits<T, SYCLDevice> {
  using packet_type = T;
  enum {
    Size = 1,
    Supported = 1,
    has_abs = 1,
    has_sqrt = 1,
    has_sin = 1,
    has_cos = 1,
    has_add = 1,
    has_sub = 1,
    has_mul = 1,
    has_div = 1,
    has_mad = 1,
    has_dot = 1,
    has_length = 1,
    has_min = 1,
    has_max = 1
  };
};

template <>
struct Packet_traits<float, SYCLDevice> {
  using packet_type = cl::sycl::cl_float4;
  enum {
    Size = 4,
    Supported = 1,
    has_abs = 1,
    has_sqrt = 1,
    has_sin = 1,
    has_cos = 1,
    has_add = 1,
    has_sub = 1,
    has_mul = 1,
    has_div = 1,
    has_mad = 1,
    has_dot = 1,
    has_length = 1,
    has_min = 1,
    has_max = 1
  };
};

}  // blas

#endif
