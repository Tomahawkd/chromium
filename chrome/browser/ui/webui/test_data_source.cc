// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/webui/test_data_source.h"

#include <memory>

#include "base/base_paths.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/memory/ref_counted_memory.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "base/task/post_task.h"
#include "base/task/task_traits.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/url_constants.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/url_data_source.h"
#include "content/public/common/url_constants.h"

namespace {
const char kModuleQuery[] = "module=";
}  // namespace

TestDataSource::TestDataSource(std::string root) {
  base::FilePath test_data;
  CHECK(base::PathService::Get(chrome::DIR_TEST_DATA, &test_data));
  src_root_ = test_data.AppendASCII(root).NormalizePathSeparators();
  DCHECK(test_data.IsParent(src_root_));

  base::FilePath exe_dir;
  base::PathService::Get(base::DIR_EXE, &exe_dir);
  gen_root_ = exe_dir.AppendASCII("gen/chrome/test/data/" + root)
                  .NormalizePathSeparators();
  DCHECK(exe_dir.IsParent(gen_root_));
}

std::string TestDataSource::GetSource() {
  return "test";
}

void TestDataSource::StartDataRequest(
    const GURL& url,
    const content::WebContents::Getter& wc_getter,
    const content::URLDataSource::GotDataCallback& callback) {
  const std::string path = content::URLDataSource::URLToRequestPath(url);
  base::PostTask(
      FROM_HERE,
      {base::ThreadPool(), base::MayBlock(), base::TaskPriority::USER_BLOCKING},
      base::BindOnce(&TestDataSource::ReadFile, base::Unretained(this), path,
                     callback));
}

std::string TestDataSource::GetMimeType(const std::string& path) {
  if (base::EndsWith(path, ".html", base::CompareCase::INSENSITIVE_ASCII) ||
      base::StartsWith(GetURLForPath(path).query(), kModuleQuery,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    // Direct request for HTML, or autogenerated HTML response for module query.
    return "text/html";
  }
  // The test data source currently only serves HTML and JS.
  CHECK(base::EndsWith(path, ".js", base::CompareCase::INSENSITIVE_ASCII))
      << "Tried to read file with unexpected type from test data source: "
      << path;
  return "application/javascript";
}

bool TestDataSource::ShouldServeMimeTypeAsContentTypeHeader() {
  return true;
}

bool TestDataSource::AllowCaching() {
  return false;
}

std::string TestDataSource::GetContentSecurityPolicyScriptSrc() {
  return "script-src chrome://* 'self';";
}

GURL TestDataSource::GetURLForPath(const std::string& path) {
  return GURL(std::string(content::kChromeUIScheme) + "://" + GetSource() +
              "/" + path);
}

void TestDataSource::ReadFile(
    const std::string& path,
    const content::URLDataSource::GotDataCallback& callback) {
  std::string content;

  GURL url = GetURLForPath(path);
  CHECK(url.is_valid());
  if (base::StartsWith(url.query(), kModuleQuery,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    std::string js_path = url.query().substr(strlen(kModuleQuery));

    base::FilePath file_path =
        src_root_.Append(base::FilePath::FromUTF8Unsafe(js_path));
    // Do some basic validation of the JS file path provided in the query.
    CHECK_EQ(file_path.Extension(), FILE_PATH_LITERAL(".js"));

    base::FilePath file_path2 =
        gen_root_.Append(base::FilePath::FromUTF8Unsafe(js_path));
    CHECK(base::PathExists(file_path) || base::PathExists(file_path2))
        << url.spec() << "=" << file_path.value();
    content = "<script type=\"module\" src=\"" + js_path + "\"></script>";
  } else {
    // Try the |src_root_| folder first.
    base::FilePath file_path =
        src_root_.Append(base::FilePath::FromUTF8Unsafe(path));
    if (base::PathExists(file_path)) {
      CHECK(base::ReadFileToString(file_path, &content))
          << url.spec() << "=" << file_path.value();
    } else {
      // Then try the |gen_root_| folder, covering cases where the test file is
      // generated at build time.
      base::FilePath file_path =
          gen_root_.Append(base::FilePath::FromUTF8Unsafe(path));
      CHECK(base::ReadFileToString(file_path, &content))
          << url.spec() << "=" << file_path.value();
    }
  }

  scoped_refptr<base::RefCountedString> response =
      base::RefCountedString::TakeString(&content);
  callback.Run(response.get());
}
