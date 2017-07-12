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
 *  @filename blas1_trees.hpp
 *
 **************************************************************************/

#ifndef BLAS1_TREE_EXPR_HPP_TEJQWMOC
#define BLAS1_TREE_EXPR_HPP_TEJQWMOC

#include <stdexcept>
#include <vector>

#include <operations/blas_operators.hpp>
#include <views/view_sycl.hpp>

namespace blas {

/*! Reduction.
 * @brief Implements the reduction operation for assignments (in the form y = x)
 *  with y a scalar and x a subexpression tree.
 */
template <typename Functor, class LHS, class RHS>
struct ReductionExpr {
  using value_type = typename RHS::value_type;

  LHS l;
  RHS r;

  ReductionExpr(LHS &_l, RHS &_r)
      : l(_l), r(_r) {}

  size_t getSize() { return r.getSize(); }
};

template <typename Functor, typename LHS, typename RHS>
ReductionExpr<Functor, LHS, RHS> make_ReductionExpr(LHS &l, RHS &r) {
  return ReductionExpr<Functor, LHS, RHS>(l, r);
}

template <typename LHS, typename RHS>
auto make_addReductionExpr(LHS &l, RHS &r)
    -> decltype(make_ReductionExpr<addOp2_struct>(l, r)) {
  return make_ReductionExpr<addOp2_struct>(l, r);
}

template <typename LHS, typename RHS>
auto make_prdReductionExpr(LHS &l, RHS &r)
    -> decltype(make_ReductionExpr<prdOp2_struct>(l, r)) {
  return make_ReductionExpr<prdOp2_struct>(l, r);
}

template <typename LHS, typename RHS>
auto make_addAbsReductionExpr(LHS &l, RHS &r)
    -> decltype(make_ReductionExpr<addAbsOp2_struct>(l, r)) {
  return make_ReductionExpr<addAbsOp2_struct>(l, r);
}

template <typename LHS, typename RHS>
auto make_maxIndReductionExpr(LHS &l, RHS &r)
    -> decltype(make_ReductionExpr<maxIndOp2_struct>(l, r)) {
  return make_ReductionExpr<maxIndOp2_struct>(l, r);
}

template <typename LHS, typename RHS>
auto make_minIndReductionExpr(LHS &l, RHS &r)
    -> decltype(make_ReductionExpr<minIndOp2_struct>(l, r)) {
  return make_ReductionExpr<minIndOp2_struct>(l, r);
}

/*!
@brief Template function for constructing expression nodes based on input
tempalte and function arguments. Non-specialised case for N reference
subexpressions.
@tparam expressionT Expression type of the expression node.
@tparam subexprsTN Subexpression types of the oeration node.
@param subexpressions Reference subexpressions of the expression node.
@return Constructed expression node.
*/
template <template <class...> class expressionT, typename... subexprsTN>
expressionT<subexprsTN...> make_expr(subexprsTN &... subexprs) {
  return expressionT<subexprsTN...>(subexprs...);
}

/*!
@brief Template function for constructing expression nodes based on input
tempalte and function arguments. Specialised case for an operator and N
reference subexpressions.
@tparam expressionT Expression type of the expression node.
@tparam exprT Expr type of the expression node.
@tparam subexprsTN subexpression types of the expression node.
@param Subexpressions Reference subexpressions of the expression node.
@return Constructed expression node.
*/
template <template <class...> class expressionT, typename exprT,
          typename... subexprsTN>
expressionT<exprT, subexprsTN...> make_expr(subexprsTN &... subexprs) {
  return expressionT<exprT, subexprsTN...>(subexprs...);
}

/*!
@brief Template function for constructing expression nodes based on input
tempalte and function arguments. Specialised case for an expression, a single by
value subexpression and N reference subexpressions.
@tparam expressionT Expression type of the expression node.
@tparam exprT Expr type of the expression node.
@tparam subexprT0 Subexpression type of the first by value subexpression of the
expression node.
@tparam subexprsTN Subexpression types of the subsequent reference
subexpressions of
the expression node.
@param subexpr0 First by value subexpression of the expression node.
@param subexprs Subsequent reference subexpressions of the expression node.
@return Constructed expression node.
*/
template <template <class...> class expressionT, typename exprT,
          typename subexprT0, typename... subexprsTN>
expressionT<exprT, subexprT0, subexprsTN...> make_expr(
    subexprT0 subexpr0, subexprsTN &... subexprs) {
  return expressionT<exprT, subexprT0, subexprsTN...>(subexpr0, subexprs...);
}

}  // namespace blas

#endif
