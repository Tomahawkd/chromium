// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/store_kit/store_kit_coordinator.h"

#import <StoreKit/StoreKit.h>

#import "base/test/ios/wait_util.h"
#import "ios/chrome/test/fakes/fake_ui_view_controller.h"
#import "ios/chrome/test/scoped_key_window.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gtest_mac.h"
#include "testing/platform_test.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

// Test fixture for StoreKitCoordinator class.
class StoreKitCoordinatorTest : public PlatformTest {
 protected:
  StoreKitCoordinatorTest()
      : base_view_controller_([[UIViewController alloc] init]),
        coordinator_([[StoreKitCoordinator alloc]
            initWithBaseViewController:base_view_controller_]) {
    [scoped_key_window_.Get() setRootViewController:base_view_controller_];
  }

  ~StoreKitCoordinatorTest() override {
    // Make sure StoreKit has been dismissed.
    if (base_view_controller_.presentedViewController) {
      [coordinator_ stop];
      EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
          base::test::ios::kWaitForActionTimeout, ^bool {
            return !base_view_controller_.presentedViewController;
          }));
    }
  }

  UIViewController* base_view_controller_;
  StoreKitCoordinator* coordinator_;
  ScopedKeyWindow scoped_key_window_;
};

// Tests that StoreKitCoordinator presents SKStoreProductViewController when
// openAppStoreWithParameters is called.
TEST_F(StoreKitCoordinatorTest, OpenStoreWithParamsPresentViewController) {
  NSDictionary* product_params = @{
    SKStoreProductParameterITunesItemIdentifier : @"TestITunesItemIdentifier",
    SKStoreProductParameterAffiliateToken : @"TestToken"
  };
  [coordinator_ openAppStoreWithParameters:product_params];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  EXPECT_NSEQ(product_params, coordinator_.iTunesProductParameters);

  EXPECT_EQ([SKStoreProductViewController class],
            [base_view_controller_.presentedViewController class]);
  [coordinator_ stop];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return !base_view_controller_.presentedViewController;
      }));

  EXPECT_FALSE(base_view_controller_.presentedViewController);
}

// Tests that StoreKitCoordinator presents SKStoreProductViewController when
// openAppStore is called.
TEST_F(StoreKitCoordinatorTest, OpenStorePresentViewController) {
  NSString* kTestITunesItemIdentifier = @"TestITunesItemIdentifier";
  NSDictionary* product_params = @{
    SKStoreProductParameterITunesItemIdentifier : kTestITunesItemIdentifier,
  };
  [coordinator_ openAppStore:kTestITunesItemIdentifier];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  EXPECT_NSEQ(product_params, coordinator_.iTunesProductParameters);

  EXPECT_EQ([SKStoreProductViewController class],
            [base_view_controller_.presentedViewController class]);

  [coordinator_ stop];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return !base_view_controller_.presentedViewController;
      }));

  EXPECT_FALSE(base_view_controller_.presentedViewController);
}

// Tests that when there is a SKStoreProductViewController presented, starting
// the coordinator doesn't present new view controller.
TEST_F(StoreKitCoordinatorTest, NoOverlappingStoreKitsPresented) {
  NSString* kTestITunesItemIdentifier = @"TestITunesItemIdentifier";
  coordinator_.iTunesProductParameters = @{
    SKStoreProductParameterITunesItemIdentifier : kTestITunesItemIdentifier,
  };
  [coordinator_ start];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  EXPECT_EQ([SKStoreProductViewController class],
            [base_view_controller_.presentedViewController class]);

  UIViewController* presented_controller =
      base_view_controller_.presentedViewController;

  [coordinator_ start];
  // Verify that that presented view controlled is not changed.
  EXPECT_NSEQ(presented_controller,
              base_view_controller_.presentedViewController);

  [coordinator_ stop];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return !base_view_controller_.presentedViewController;
      }));

  EXPECT_FALSE(base_view_controller_.presentedViewController);

  [coordinator_ start];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  // After reseting the view controller, a new storekit view should be
  // presented.
  EXPECT_EQ([SKStoreProductViewController class],
            [base_view_controller_.presentedViewController class]);
  EXPECT_NSNE(presented_controller,
              base_view_controller_.presentedViewController);
}

// Tests that if the base view controller is presenting any view controller,
// starting the coordinator doesn't present new view controller.
// TODO:(crbug.com/968514): Re-enable this test on devices.
#if TARGET_OS_SIMULATOR
#define MAYBE_NoOverlappingPresentedViewControllers \
  NoOverlappingPresentedViewControllers
#else
#define MAYBE_NoOverlappingPresentedViewControllers \
  FLAKY_NoOverlappingPresentedViewControllers
#endif
TEST_F(StoreKitCoordinatorTest, MAYBE_NoOverlappingPresentedViewControllers) {
  NSString* kTestITunesItemIdentifier = @"TestITunesItemIdentifier";
  coordinator_.iTunesProductParameters = @{
    SKStoreProductParameterITunesItemIdentifier : kTestITunesItemIdentifier,
  };
  EXPECT_FALSE(base_view_controller_.presentedViewController);
  UIViewController* dummy_view_controller = [[UIViewController alloc] init];
  [base_view_controller_ presentViewController:dummy_view_controller
                                      animated:NO
                                    completion:nil];
  EXPECT_NSEQ(dummy_view_controller,
              base_view_controller_.presentedViewController);

  [coordinator_ start];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  // Verify that that presented view controlled is not changed.
  EXPECT_NSEQ(dummy_view_controller,
              base_view_controller_.presentedViewController);
  [coordinator_ stop];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return !base_view_controller_.presentedViewController;
      }));

  EXPECT_FALSE(base_view_controller_.presentedViewController);

  [coordinator_ start];
  EXPECT_TRUE(base::test::ios::WaitUntilConditionOrTimeout(
      base::test::ios::kWaitForActionTimeout, ^bool {
        return base_view_controller_.presentedViewController;
      }));

  // After reseting the view controller, a new storekit view should be
  // presented.
  EXPECT_EQ([SKStoreProductViewController class],
            [base_view_controller_.presentedViewController class]);
}
