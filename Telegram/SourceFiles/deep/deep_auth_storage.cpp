#include "deep/deep_auth_storage.h"

#include "deep/deep_session.h"
#include "main/main_account.h"
#include "storage/storage_account.h"

namespace Deep {
namespace {

constexpr auto kJwtKey = "teledeep.jwt";
constexpr auto kUserIdKey = "teledeep.uid";

} // namespace

void persistAuth(not_null<Main::Account*> account) {
	const auto &session = Session::instance();
	if (!session.hasToken()) {
		return;
	}
	auto &local = account->local();
	local.writePref<QByteArray>(kJwtKey, session.token().toUtf8());
	local.writePref<QByteArray>(kUserIdKey, session.userId().toUtf8());
}

void restoreAuth(not_null<Main::Account*> account) {
	if (Session::instance().hasToken()) {
		return;
	}
	auto &local = account->local();
	const auto jwt = local.readPref<QByteArray>(kJwtKey);
	if (jwt.isEmpty()) {
		return;
	}
	const auto uid = local.readPref<QByteArray>(kUserIdKey);
	Session::instance().setToken(QString::fromUtf8(jwt));
	Session::instance().setUserId(QString::fromUtf8(uid));
}

void clearAuth(not_null<Main::Account*> account) {
	Session::instance().clear();
	account->local().clearPref(kJwtKey);
	account->local().clearPref(kUserIdKey);
}

} // namespace Deep
