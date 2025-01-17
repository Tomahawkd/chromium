// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "google_apis/gaia/core_account_id.h"

#include "base/logging.h"

namespace {
// Returns whether the string looks like an email (the test is
// crude an only checks whether it includes an '@').
bool IsEmail(const std::string& string) {
  return string.find('@') != std::string::npos;
}
}  // anonymous namespace

CoreAccountId::CoreAccountId() = default;

CoreAccountId::CoreAccountId(const CoreAccountId&) = default;

CoreAccountId::CoreAccountId(CoreAccountId&&) noexcept = default;

CoreAccountId::~CoreAccountId() = default;

CoreAccountId& CoreAccountId::operator=(const CoreAccountId&) = default;

CoreAccountId& CoreAccountId::operator=(CoreAccountId&&) noexcept = default;

// static
CoreAccountId CoreAccountId::FromGaiaId(const std::string& gaia_id) {
  if (gaia_id.empty())
    return CoreAccountId();

  DCHECK(!IsEmail(gaia_id))
      << "Expected a Gaia ID and got an email [actual = " << gaia_id << "]";
  return CoreAccountId::FromString(gaia_id);
}

// static
CoreAccountId CoreAccountId::FromEmail(const std::string& email) {
  if (email.empty())
    return CoreAccountId();

  DCHECK(IsEmail(email)) << "Expected an email [actual = " << email << "]";
  return CoreAccountId::FromString(email);
}

// static
CoreAccountId CoreAccountId::FromString(const std::string value) {
  CoreAccountId account_id;
  account_id.id = value;
  return account_id;
}

bool CoreAccountId::empty() const {
  return id.empty();
}

bool operator<(const CoreAccountId& lhs, const CoreAccountId& rhs) {
  return lhs.id < rhs.id;
}

bool operator==(const CoreAccountId& lhs, const CoreAccountId& rhs) {
  return lhs.id == rhs.id;
}

bool operator!=(const CoreAccountId& lhs, const CoreAccountId& rhs) {
  return lhs.id != rhs.id;
}

std::ostream& operator<<(std::ostream& out, const CoreAccountId& a) {
  return out << a.id;
}

std::vector<std::string> ToStringList(
    const std::vector<CoreAccountId>& account_ids) {
  std::vector<std::string> account_ids_string;
  for (const auto& account_id : account_ids)
    account_ids_string.push_back(account_id.id);
  return account_ids_string;
}