// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "exe_path.h"

#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "build_config.h"

#if defined(OS_MACOSX)
#include <mach-o/dyld.h>

base::FilePath GetExePath() {
  // Executable path can have relative references ("..") depending on
  // how the app was launched.
  uint32_t executable_length = 0;
  _NSGetExecutablePath(NULL, &executable_length);
  DCHECK_GT(executable_length, 1u);
  std::string executable_path;
  int rv = _NSGetExecutablePath(
      base::WriteInto(&executable_path, executable_length),
                      &executable_length);
  DCHECK_EQ(rv, 0);

  // _NSGetExecutablePath may return paths containing ./ or ../ which makes
  // FilePath::DirName() work incorrectly, convert it to absolute path so that
  // paths such as DIR_SOURCE_ROOT can work, since we expect absolute paths to
  // be returned here.
  return base::MakeAbsoluteFilePath(base::FilePath(executable_path));
}

#elif defined(OS_WIN)

#error

#else

base::FilePath GetExePath() {
  base::FilePath result;
  const char kProcSelfExe[] = "/proc/self/exe";
  if (!ReadSymbolicLink(base::FilePath(kProcSelfExe), &result)) {
    NOTREACHED() << "Unable to resolve " << kProcSelfExe << ".";
    return base::FilePath();
  }
  return result;
}

#endif