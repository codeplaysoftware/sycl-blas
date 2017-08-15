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
 *  @filename clblast_benchmark.cpp
 *
 **************************************************************************/

#include "blas_benchmark.hpp"

#include <complex>
#include <vector>

#include <clblast.h>

#include "clwrap.hpp"

#define UNPACK_PARAM using ScalarT = TypeParam;

class ClBlastBenchmarker {
  Context context;

 public:
  ClBlastBenchmarker() : context() {}

  BENCHMARK_FUNCTION(scal_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alpha(2.4367453465);
      MemBuffer<ScalarT> buf1(context, size);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 1, [&]() {
        clblast::Scal<ScalarT>(size, alpha, buf1.dev(), 0, 1, context._queue(),
                               &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(axpy_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alpha(2.4367453465);
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf1(context, size);
      MemBuffer<ScalarT> buf2(context, size);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Axpy<ScalarT>(size, alpha, buf1.dev(), 0, 1, buf2.dev(), 0, 1,
                               context._queue(), &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(asum_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT vr;
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf1(context, size);
      MemBuffer<ScalarT, CL_MEM_READ_ONLY> bufr(context, &vr, 1);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Asum<ScalarT>(size, bufr.dev(), 0, buf1.dev(), 0, 1,
                               context._queue(), &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(nrm2_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT vr;
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf1(context, size);
      MemBuffer<ScalarT, CL_MEM_READ_ONLY> bufr(context, &vr, 1);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Nrm2<ScalarT>(size, bufr.dev(), 0, buf1.dev(), 0, 1,
                               context._queue(), &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(dot_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT vr;
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf1(context, size);
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf2(context, size);
      MemBuffer<ScalarT, CL_MEM_READ_ONLY> bufr(context, &vr, 1);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Dot<ScalarT>(size, bufr.dev(), 0, buf1.dev(), 0, 1, buf2.dev(),
                              0, 1, context._queue(), &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(iamax_bench) {
    UNPACK_PARAM;
    double flops;
    {
      int vi;
      MemBuffer<ScalarT, CL_MEM_WRITE_ONLY> buf1(context, size);
      MemBuffer<int, CL_MEM_READ_ONLY> buf_i(context, &vi, 1);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Amax<ScalarT>(size, buf_i.dev(), 0, buf1.dev(), 0, 1,
                               context._queue(), &event);
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
      });
    }
    return flops;
  }

  // not supported at current release yet
  /* BENCHMARK_FUNCTION(iamin_bench) { */
  /*   UNPACK_PARAM; */
  /*   double flops; */
  /*   { */
  /*     int vi; */
  /*     MemBuffer<ScalarT> buf1(context, size); */
  /*     MemBuffer<int> buf_i(context, &vi, 1); */

  /*     cl_event event; */
  /*     flops = benchmark<>::measure(no_reps, [&]() { */
  /*       clblast::Amin<ScalarT>(size, buf_i.dev(), 0, buf1.dev(), 0, 1, */
  /*                               context._queue(), &event); */
  /*       clWaitForEvents(1, &event); */
  /*       clReleaseEvent(event); */
  /*     }); */
  /*   } */
  /*   return flops; */
  /* } */

  BENCHMARK_FUNCTION(scal2op_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alpha(2.4367453465);
      MemBuffer<ScalarT> buf1(context, size);
      MemBuffer<ScalarT> buf2(context, size);

      cl_event events[2];
      flops = benchmark<>::measure(no_reps, size * 2, [&]() {
        clblast::Scal<ScalarT>(size, alpha, buf1.dev(), 0, 1, context._queue(),
                               &events[0]);
        clblast::Scal<ScalarT>(size, alpha, buf2.dev(), 0, 1, context._queue(),
                               &events[1]);
        clWaitForEvents(2, events);
        clReleaseEvent(events[0]);
        clReleaseEvent(events[1]);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(scal3op_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alpha(2.4367453465);
      MemBuffer<ScalarT> buf1(context, size);
      MemBuffer<ScalarT> buf2(context, size);
      MemBuffer<ScalarT> buf3(context, size);

      cl_event events[3];
      flops = benchmark<>::measure(no_reps, size * 3, [&]() {
        clblast::Scal<ScalarT>(size, alpha, buf1.dev(), 0, 1, context._queue(),
                               &events[0]);
        clblast::Scal<ScalarT>(size, alpha, buf2.dev(), 0, 1, context._queue(),
                               &events[1]);
        clblast::Scal<ScalarT>(size, alpha, buf3.dev(), 0, 1, context._queue(),
                               &events[2]);
        clWaitForEvents(3, events);
        clReleaseEvent(events[0]);
        clReleaseEvent(events[1]);
        clReleaseEvent(events[2]);
      });
    }
    return flops;
  }

  BENCHMARK_FUNCTION(axpy3op_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alphas[] = {1.78426458744, 2.187346575843, 3.78164387328};
      size_t offsets[] = {0, size, size * 2};
      MemBuffer<ScalarT, CL_MEM_READ_ONLY> bufsrc(context, size * 3);
      MemBuffer<ScalarT> bufdst(context, size * 3);

      cl_event event;
      flops = benchmark<>::measure(no_reps, size * 3 * 2, [&]() {
        clblast::AxpyBatched<ScalarT>(size, alphas, bufsrc.dev(), offsets, 1,
                                      bufdst.dev(), offsets, 1, 3,
                                      context._queue(), &event);
      });
      clWaitForEvents(1, &event);
      clReleaseEvent(event);
    }
    return flops;
  }

  BENCHMARK_FUNCTION(blas1_bench) {
    UNPACK_PARAM;
    double flops;
    {
      ScalarT alpha(3.135345123);
      MemBuffer<ScalarT> buf1(context, size);
      MemBuffer<ScalarT> buf2(context, size);
      ScalarT vr[3];
      size_t vi;
      MemBuffer<ScalarT, CL_MEM_READ_ONLY> bufr(context, vr, 3);
      MemBuffer<size_t, CL_MEM_READ_ONLY> buf_i(context, &vi, 1);

      cl_event events[5];
      flops = benchmark<>::measure(no_reps, size * 12, [&]() {
        clblast::Axpy<ScalarT>(size, alpha, buf1.dev(), 0, 1, buf2.dev(), 0, 1,
                               context._queue(), &events[0]);
        clblast::Asum<ScalarT>(size, bufr.dev(), 0, buf2.dev(), 0, 1,
                               context._queue(), &events[1]);
        clblast::Dot<ScalarT>(size, bufr.dev(), 1, buf1.dev(), 0, 1, buf2.dev(),
                              0, 1, context._queue(), &events[2]);
        clblast::Nrm2<ScalarT>(size, bufr.dev(), 2, buf1.dev(), 0, 1,
                               context._queue(), &events[3]);
        clblast::Amax<ScalarT>(size, buf_i.dev(), 0, buf1.dev(), 0, 1,
                               context._queue(), &events[4]);
        clWaitForEvents(5, events);
        for (int i = 0; i < 5; ++i) clReleaseEvent(events[i]);
      });
    }
    return flops;
  }
};

BENCHMARK_MAIN_BEGIN(1 << 1, 1 << 24, 10);
ClBlastBenchmarker blasbenchmark;

BENCHMARK_REGISTER_FUNCTION("scal_float", scal_bench<float>);
BENCHMARK_REGISTER_FUNCTION("scal_double", scal_bench<double>);

BENCHMARK_REGISTER_FUNCTION("axpy_float", axpy_bench<float>);
BENCHMARK_REGISTER_FUNCTION("axpy_double", axpy_bench<double>);

BENCHMARK_REGISTER_FUNCTION("asum_float", asum_bench<float>);
BENCHMARK_REGISTER_FUNCTION("asum_double", asum_bench<double>);

BENCHMARK_REGISTER_FUNCTION("nrm2_float", nrm2_bench<float>);
BENCHMARK_REGISTER_FUNCTION("nrm2_double", nrm2_bench<double>);

BENCHMARK_REGISTER_FUNCTION("dot_float", dot_bench<float>);
BENCHMARK_REGISTER_FUNCTION("dot_double", dot_bench<double>);

BENCHMARK_REGISTER_FUNCTION("iamax_float", iamax_bench<float>);
BENCHMARK_REGISTER_FUNCTION("iamax_double", iamax_bench<double>);

/* BENCHMARK_REGISTER_FUNCTION("iamin_float", iamin_bench<float>); */
/* BENCHMARK_REGISTER_FUNCTION("iamin_double", iamin_bench<double>); */

BENCHMARK_REGISTER_FUNCTION("scal2op_float", scal2op_bench<float>);
BENCHMARK_REGISTER_FUNCTION("scal2op_double", scal2op_bench<double>);

BENCHMARK_REGISTER_FUNCTION("scal3op_float", scal3op_bench<float>);
BENCHMARK_REGISTER_FUNCTION("scal3op_double", scal3op_bench<double>);

BENCHMARK_REGISTER_FUNCTION("axpy3op_float", axpy3op_bench<float>);
BENCHMARK_REGISTER_FUNCTION("axpy3op_double", axpy3op_bench<double>);

BENCHMARK_REGISTER_FUNCTION("blas1_float", blas1_bench<float>);
BENCHMARK_REGISTER_FUNCTION("blas1_double", blas1_bench<double>);

BENCHMARK_MAIN_END();
