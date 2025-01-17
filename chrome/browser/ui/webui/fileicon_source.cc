// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/webui/fileicon_source.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted_memory.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/browser_process.h"
#include "chrome/common/webui_url_constants.h"
#include "net/base/escape.h"
#include "net/base/url_util.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/webui/web_ui_util.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "url/gurl.h"

namespace {

typedef std::map<std::string, IconLoader::IconSize> QueryIconSizeMap;

// The path used in internal URLs to file icon data.
const char kFileIconPath[] = "fileicon";

// URL parameter specifying icon size.
const char kIconSizeParameter[] = "iconsize";

// URL parameter specifying the file path for which to get an icon.
const char kPathParameter[] = "path";

// URL parameter specifying scale factor.
const char kScaleFactorParameter[] = "scale";

IconLoader::IconSize SizeStringToIconSize(const std::string& size_string) {
  if (size_string == "small") return IconLoader::SMALL;
  if (size_string == "large") return IconLoader::LARGE;
  // We default to NORMAL if we don't recognize the size_string. Including
  // size_string=="normal".
  return IconLoader::NORMAL;
}

void ParseQueryParams(const std::string& path,
                      base::FilePath* file_path,
                      float* scale_factor,
                      IconLoader::IconSize* icon_size) {
  GURL request = GURL(chrome::kChromeUIFileiconURL).Resolve(path);
  for (net::QueryIterator it(request); !it.IsAtEnd(); it.Advance()) {
    std::string key = it.GetKey();
    if (key == kPathParameter) {
      *file_path = base::FilePath::FromUTF8Unsafe(it.GetUnescapedValue())
                       .NormalizePathSeparators();
    } else if (key == kIconSizeParameter) {
      *icon_size = SizeStringToIconSize(it.GetValue());
    } else if (key == kScaleFactorParameter) {
      webui::ParseScaleFactor(it.GetValue(), scale_factor);
    }
  }
}

}  // namespace

FileIconSource::IconRequestDetails::IconRequestDetails() : scale_factor(1.0f) {
}

FileIconSource::IconRequestDetails::IconRequestDetails(
    const IconRequestDetails& other) = default;

FileIconSource::IconRequestDetails::~IconRequestDetails() {
}

FileIconSource::FileIconSource() {}

FileIconSource::~FileIconSource() {}

void FileIconSource::FetchFileIcon(
    const base::FilePath& path,
    float scale_factor,
    IconLoader::IconSize icon_size,
    const content::URLDataSource::GotDataCallback& callback) {
  IconManager* im = g_browser_process->icon_manager();
  gfx::Image* icon = im->LookupIconFromFilepath(path, icon_size);

  if (icon) {
    scoped_refptr<base::RefCountedBytes> icon_data(new base::RefCountedBytes);
    gfx::PNGCodec::EncodeBGRASkBitmap(
        icon->ToImageSkia()->GetRepresentation(scale_factor).GetBitmap(), false,
        &icon_data->data());

    callback.Run(icon_data.get());
  } else {
    // Attach the ChromeURLDataManager request ID to the history request.
    IconRequestDetails details;
    details.callback = callback;
    details.scale_factor = scale_factor;

    // Icon was not in cache, go fetch it slowly.
    im->LoadIcon(path,
                 icon_size,
                 base::Bind(&FileIconSource::OnFileIconDataAvailable,
                            base::Unretained(this), details),
                 &cancelable_task_tracker_);
  }
}

std::string FileIconSource::GetSource() {
  return kFileIconPath;
}

void FileIconSource::StartDataRequest(
    const GURL& url,
    const content::WebContents::Getter& wc_getter,
    const content::URLDataSource::GotDataCallback& callback) {
  const std::string path = content::URLDataSource::URLToRequestPath(url);
  base::FilePath file_path;
  IconLoader::IconSize icon_size = IconLoader::NORMAL;
  float scale_factor = 1.0f;
  // TODO(crbug/1009127): Make ParseQueryParams take GURL.
  ParseQueryParams(path, &file_path, &scale_factor, &icon_size);
  FetchFileIcon(file_path, scale_factor, icon_size, callback);
}

std::string FileIconSource::GetMimeType(const std::string&) {
  // Rely on image decoder inferring the correct type.
  return std::string();
}

bool FileIconSource::AllowCaching() {
  return false;
}

void FileIconSource::OnFileIconDataAvailable(const IconRequestDetails& details,
                                             gfx::Image icon) {
  if (!icon.IsEmpty()) {
    scoped_refptr<base::RefCountedBytes> icon_data(new base::RefCountedBytes);
    gfx::PNGCodec::EncodeBGRASkBitmap(
        icon.ToImageSkia()->GetRepresentation(details.scale_factor).GetBitmap(),
        false, &icon_data->data());

    details.callback.Run(icon_data.get());
  } else {
    // TODO(glen): send a dummy icon.
    details.callback.Run(nullptr);
  }
}
