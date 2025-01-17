// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_OPTIMIZATION_GUIDE_OPTIMIZATION_GUIDE_ENUMS_H_
#define COMPONENTS_OPTIMIZATION_GUIDE_OPTIMIZATION_GUIDE_ENUMS_H_

namespace optimization_guide {

// The types of decisions that can be made for an optimization type.
//
// Keep in sync with OptimizationGuideOptimizationTypeDecision in enums.xml.
enum class OptimizationTypeDecision {
  kUnknown,
  // The optimization type was allowed for the page load by an optimization
  // filter for the type.
  kAllowedByOptimizationFilter,
  // The optimization type was not allowed for the page load by an optimization
  // filter for the type.
  kNotAllowedByOptimizationFilter,
  // An optimization filter for that type was on the device but was not loaded
  // in time to make a decision. There is no guarantee that had the filter been
  // loaded that the page load would have been allowed for the optimization
  // type.
  kHadOptimizationFilterButNotLoadedInTime,
  // The optimization type was allowed for the page load based on a hint.
  kAllowedByHint,
  // A hint that matched the page load was present but the optimization type was
  // not allowed to be applied.
  kNotAllowedByHint,
  // A hint was available but there was not a page hint within that hint that
  // matched the page load.
  kNoMatchingPageHint,
  // A hint that matched the page load was on the device but was not loaded in
  // time to make a decision. There is no guarantee that had the hint been
  // loaded that the page load would have been allowed for the optimization
  // type.
  kHadHintButNotLoadedInTime,
  // No hints were available in the cache that matched the page load.
  kNoHintAvailable,
  // The OptimizationGuideDecider was not initialized yet.
  kDeciderNotInitialized,
  // A fetch to get the hint for the page load from the remote Optimization
  // Guide Service was started, but was not available in time to make a
  // decision.
  kHintFetchStartedButNotAvailableInTime,

  // Add new values above this line.
  kMaxValue = kHintFetchStartedButNotAvailableInTime,
};

// The types of decisions that can be made for an optimization target.
//
// Keep in sync with OptimizationGuideOptimizationTargetDecision in enums.xml.
enum class OptimizationTargetDecision {
  kUnknown,
  // The page load does not match the optimization target.
  kPageLoadDoesNotMatch,
  // The page load matches the optimization target.
  kPageLoadMatches,
  // The model needed to make the target decision was not available on the
  // client.
  kModelNotAvailableOnClient,
  // The page load is part of a model prediction holdback where all decisions
  // will return |OptimizationGuideDecision::kFalse| in an attempt to not taint
  // the data for understanding the production recall of the model.
  kModelPredictionHoldback,
  // The OptimizationGuideDecider was not initialized yet.
  kDeciderNotInitialized,

  // Add new values above this line.
  kMaxValue = kDeciderNotInitialized,
};

}  // namespace optimization_guide

#endif  // COMPONENTS_OPTIMIZATION_GUIDE_OPTIMIZATION_GUIDE_ENUMS_H_
