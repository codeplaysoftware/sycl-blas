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
 *  @filename blas_pointer_struct.hpp
 *
 **************************************************************************/

#ifndef BLAS_POINTER_STRUCT_HPP
#define BLAS_POINTER_STRUCT_HPP

template <typename T>
struct MakeHostPointer {
  using type = cl::sycl::buffer<T, 1> *;
};

template <typename T>
struct MakeDevicePointer {
  using type =
      cl::sycl::accessor<T, 1, cl::sycl::access::mode::read_write,
                         cl::sycl::access::target::global_buffer,
                         cl::sycl::codeplay::access::placeholder::true_t>;
};

#endif  // BLAS_POINTER_STRUCT_HPP
