// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_INFOBARS_COORDINATORS_INFOBAR_TRANSLATE_MEDIATOR_H_
#define IOS_CHROME_BROWSER_UI_INFOBARS_COORDINATORS_INFOBAR_TRANSLATE_MEDIATOR_H_

#import <UIKit/UIKit.h>

namespace translate {
class TranslateInfoBarDelegate;
}  // namespace translate

@protocol InfobarTranslateLanguageSelectionConsumer;
@protocol InfobarTranslateModalConsumer;

@interface InfobarTranslateMediator : NSObject

// Designated initializer. |infoBarDelegate| cannot be nil and is not retained
- (instancetype)initWithInfoBarDelegate:
    (translate::TranslateInfoBarDelegate*)infobarDelegate
    NS_DESIGNATED_INITIALIZER;
- (instancetype)init NS_UNAVAILABLE;

// The consumer for the Infobar Modal.
@property(nonatomic, weak) id<InfobarTranslateModalConsumer> modalConsumer;

// The consumer selecting the source language to be configured with this
// mediator.
@property(nonatomic, weak) id<InfobarTranslateLanguageSelectionConsumer>
    sourceLanguageSelectionConsumer;

// The consumer selecting the target language to be configured with this
// mediator.
@property(nonatomic, weak) id<InfobarTranslateLanguageSelectionConsumer>
    targetLanguageSelectionConsumer;

@end

#endif  // IOS_CHROME_BROWSER_UI_INFOBARS_COORDINATORS_INFOBAR_TRANSLATE_MEDIATOR_H_
