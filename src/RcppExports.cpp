// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// peelCpp
List peelCpp(const NumericMatrix& M, const NumericVector& y, const IntegerVector& colTypes, const double& alpha, const double& minSup);
RcppExport SEXP _subgroup_discovery_peelCpp(SEXP MSEXP, SEXP ySEXP, SEXP colTypesSEXP, SEXP alphaSEXP, SEXP minSupSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const NumericMatrix& >::type M(MSEXP);
    Rcpp::traits::input_parameter< const NumericVector& >::type y(ySEXP);
    Rcpp::traits::input_parameter< const IntegerVector& >::type colTypes(colTypesSEXP);
    Rcpp::traits::input_parameter< const double& >::type alpha(alphaSEXP);
    Rcpp::traits::input_parameter< const double& >::type minSup(minSupSEXP);
    rcpp_result_gen = Rcpp::wrap(peelCpp(M, y, colTypes, alpha, minSup));
    return rcpp_result_gen;
END_RCPP
}
// predictCpp
List predictCpp(const List& peelSteps, const NumericMatrix& M, const NumericVector& y);
RcppExport SEXP _subgroup_discovery_predictCpp(SEXP peelStepsSEXP, SEXP MSEXP, SEXP ySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const List& >::type peelSteps(peelStepsSEXP);
    Rcpp::traits::input_parameter< const NumericMatrix& >::type M(MSEXP);
    Rcpp::traits::input_parameter< const NumericVector& >::type y(ySEXP);
    rcpp_result_gen = Rcpp::wrap(predictCpp(peelSteps, M, y));
    return rcpp_result_gen;
END_RCPP
}
// simplifyCpp
List simplifyCpp(const List& peelSteps, const IntegerVector& colTypes, const size_t& boxId);
RcppExport SEXP _subgroup_discovery_simplifyCpp(SEXP peelStepsSEXP, SEXP colTypesSEXP, SEXP boxIdSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const List& >::type peelSteps(peelStepsSEXP);
    Rcpp::traits::input_parameter< const IntegerVector& >::type colTypes(colTypesSEXP);
    Rcpp::traits::input_parameter< const size_t& >::type boxId(boxIdSEXP);
    rcpp_result_gen = Rcpp::wrap(simplifyCpp(peelSteps, colTypes, boxId));
    return rcpp_result_gen;
END_RCPP
}
// indexCpp
IntegerVector indexCpp(const List& boxes, const NumericMatrix& M, const size_t& boxId);
RcppExport SEXP _subgroup_discovery_indexCpp(SEXP boxesSEXP, SEXP MSEXP, SEXP boxIdSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const List& >::type boxes(boxesSEXP);
    Rcpp::traits::input_parameter< const NumericMatrix& >::type M(MSEXP);
    Rcpp::traits::input_parameter< const size_t& >::type boxId(boxIdSEXP);
    rcpp_result_gen = Rcpp::wrap(indexCpp(boxes, M, boxId));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_subgroup_discovery_peelCpp", (DL_FUNC) &_subgroup_discovery_peelCpp, 5},
    {"_subgroup_discovery_predictCpp", (DL_FUNC) &_subgroup_discovery_predictCpp, 3},
    {"_subgroup_discovery_simplifyCpp", (DL_FUNC) &_subgroup_discovery_simplifyCpp, 3},
    {"_subgroup_discovery_indexCpp", (DL_FUNC) &_subgroup_discovery_indexCpp, 3},
    {NULL, NULL, 0}
};

RcppExport void R_init_subgroup_discovery(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
