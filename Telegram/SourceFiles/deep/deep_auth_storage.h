#pragma once

#include "base/assertion.h"

namespace Main {
class Account;
} // namespace Main

namespace Deep {

void persistAuth(not_null<Main::Account*> account);
void restoreAuth(not_null<Main::Account*> account);
void clearAuth(not_null<Main::Account*> account);

} // namespace Deep
