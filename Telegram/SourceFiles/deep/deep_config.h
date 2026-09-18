/*
 * TeleDeep — Deep Messenger backend hooks for Telegram Desktop (GPL fork).
 */
#pragma once

#include <QString>

namespace Deep {

[[nodiscard]] bool Enabled();
[[nodiscard]] QString ApiBase();
[[nodiscard]] QString ApiV1(const QString &path);
[[nodiscard]] QString webSocketUrl(const QString &token);

} // namespace Deep
