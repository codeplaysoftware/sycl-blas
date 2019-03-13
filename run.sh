#!/bin/bash

# This script runs the SYCL-BLAS tests using the provided Dockerfile.
# The intention is to provide a seamless alternative to .travis.yml, so that developers can locally test changes in a (somewhat) platform-agnostic manner without the usual delay that happens with travis.
export IMPL=COMPUTECPP
export CXX_COMPILER=g++-7
export CC_COMPILER=gcc-7
export TARGET=opencl
export GIT_SLUG="AdamHarries/sycl-blas"
export GIT_BRANCH="master"


docker build --build-arg c_compiler=${CC_COMPILER} \
    --build-arg cxx_compiler=${CXX_COMPILER} \
    --build-arg git_branch=${GIT_BRANCH} \
    --build-arg git_slug=${GIT_SLUG} \
    --build-arg impl=${IMPL} \
    --build-arg target=${TARGET} \
    -t sycl-blas .

docker run sycl-blas
