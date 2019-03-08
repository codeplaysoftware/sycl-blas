/***************************************************************************
 *  @license
 *  Copyright (C) Codeplay Software Limited
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
 *  @filename sycl_policy_handler.cpp
 *
 **************************************************************************/

#include "operations/blas_constants.h"
// the templated methods
#include "policy/sycl_policy_handler.hpp"

namespace blas {

#define INSTANTIATE_TEMPLATE_METHODS(element_t)                                \
  template element_t *PolicyHandler<codeplay_policy>::allocate<element_t>(     \
      size_t num_elements) const;                                              \
  template void PolicyHandler<codeplay_policy>::deallocate<element_t>(         \
      element_t * p) const;                                                    \
  template BufferIterator<element_t, codeplay_policy>                          \
      PolicyHandler<codeplay_policy>::get_buffer<element_t>(element_t * ptr)   \
          const;                                                               \
  template BufferIterator<element_t, codeplay_policy>                          \
  PolicyHandler<codeplay_policy>::get_buffer<element_t>(                       \
      BufferIterator<element_t, codeplay_policy> buff) const;                  \
  template typename codeplay_policy::default_accessor_t<                       \
      typename ValueType<element_t>::type, cl::sycl::access::mode::read_write> \
      PolicyHandler<codeplay_policy>::get_range_access<                        \
          cl::sycl::access::mode::read_write, element_t>(element_t * vptr);    \
                                                                               \
  template typename codeplay_policy::default_accessor_t<                       \
      typename ValueType<element_t>::type, cl::sycl::access::mode::read_write> \
  PolicyHandler<codeplay_policy>::get_range_access<                            \
      element_t, cl::sycl::access::mode::read_write>(                          \
      BufferIterator<element_t, codeplay_policy> buff);                        \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_device<element_t>(                   \
      const element_t *src, element_t *dst, size_t size);                      \
                                                                               \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_device<element_t>(                   \
      const element_t *src, BufferIterator<element_t, codeplay_policy> dst,    \
      size_t size = 0);                                                        \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_host<element_t>(                     \
      element_t * src, element_t * dst, size_t size);                          \
                                                                               \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_host<element_t>(                     \
      BufferIterator<element_t, codeplay_policy> src, element_t * dst,         \
      size_t size = 0);                                                        \
  template ptrdiff_t PolicyHandler<codeplay_policy>::get_offset<element_t>(    \
      const element_t *ptr) const;                                             \
                                                                               \
  template ptrdiff_t PolicyHandler<codeplay_policy>::get_offset<element_t>(    \
      BufferIterator<element_t, codeplay_policy> ptr) const;

INSTANTIATE_TEMPLATE_METHODS(float)
INSTANTIATE_TEMPLATE_METHODS(double)

#define INSTANTIATE_TEMPLATE_METHODS_SPECIAL(ind, val)                         \
  template Indexvalue_tuple<ind, val>                                          \
      *PolicyHandler<codeplay_policy>::allocate<Indexvalue_tuple<ind, val>>(   \
          size_t num_elements) const;                                          \
  template void                                                                \
      PolicyHandler<codeplay_policy>::deallocate<Indexvalue_tuple<ind, val>>(  \
          Indexvalue_tuple<ind, val> * p) const;                               \
  template BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy>         \
      PolicyHandler<codeplay_policy>::get_buffer<Indexvalue_tuple<ind, val>>(  \
          Indexvalue_tuple<ind, val> * ptr) const;                             \
  template BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy>         \
  PolicyHandler<codeplay_policy>::get_buffer<Indexvalue_tuple<ind, val>>(      \
      BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy> buff) const; \
  template typename codeplay_policy::default_accessor_t<                       \
      typename ValueType<Indexvalue_tuple<ind, val>>::type,                    \
      cl::sycl::access::mode::read_write>                                      \
      PolicyHandler<codeplay_policy>::get_range_access<                        \
          cl::sycl::access::mode::read_write, Indexvalue_tuple<ind, val>>(     \
          Indexvalue_tuple<ind, val> * vptr);                                  \
                                                                               \
  template typename codeplay_policy::default_accessor_t<                       \
      typename ValueType<Indexvalue_tuple<ind, val>>::type,                    \
      cl::sycl::access::mode::read_write>                                      \
  PolicyHandler<codeplay_policy>::get_range_access<                            \
      Indexvalue_tuple<ind, val>, cl::sycl::access::mode::read_write>(         \
      BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy> buff);       \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_device<Indexvalue_tuple<ind, val>>(  \
      const Indexvalue_tuple<ind, val> *src, Indexvalue_tuple<ind, val> *dst,  \
      size_t size);                                                            \
                                                                               \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_device<Indexvalue_tuple<ind, val>>(  \
      const Indexvalue_tuple<ind, val> *src,                                   \
      BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy> dst,         \
      size_t size = 0);                                                        \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_host<Indexvalue_tuple<ind, val>>(    \
      Indexvalue_tuple<ind, val> * src, Indexvalue_tuple<ind, val> * dst,      \
      size_t size);                                                            \
                                                                               \
  template typename codeplay_policy::event_t                                   \
  PolicyHandler<codeplay_policy>::copy_to_host<Indexvalue_tuple<ind, val>>(    \
      BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy> src,         \
      Indexvalue_tuple<ind, val> * dst, size_t size = 0);                      \
  template ptrdiff_t                                                           \
  PolicyHandler<codeplay_policy>::get_offset<Indexvalue_tuple<ind, val>>(      \
      const Indexvalue_tuple<ind, val> *ptr) const;                            \
                                                                               \
  template ptrdiff_t                                                           \
  PolicyHandler<codeplay_policy>::get_offset<Indexvalue_tuple<ind, val>>(      \
      BufferIterator<Indexvalue_tuple<ind, val>, codeplay_policy> ptr) const;

INSTANTIATE_TEMPLATE_METHODS_SPECIAL(float, int)
INSTANTIATE_TEMPLATE_METHODS_SPECIAL(float, long)
INSTANTIATE_TEMPLATE_METHODS_SPECIAL(float, long long)
INSTANTIATE_TEMPLATE_METHODS_SPECIAL(double, int)
INSTANTIATE_TEMPLATE_METHODS_SPECIAL(double, long)
INSTANTIATE_TEMPLATE_METHODS_SPECIAL(double, long long)

}  // namespace blas