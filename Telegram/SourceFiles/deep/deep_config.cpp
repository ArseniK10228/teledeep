#include "deep/deep_config.h"

#include "deep/deep_client_id.h"

#include <QProcessEnvironment>
#include <QUrl>
#include <QUrlQuery>

namespace Deep {

bool Enabled() {
#ifdef TELEDEEP_BUILD
	return true;
#else
	return qEnvironmentVariableIsSet("TELEDEEP")
		|| qEnvironmentVariableIsSet("TELEDEEP_API");
#endif
}

QString ApiBase() {
	const auto env = QProcessEnvironment::systemEnvironment();
	const auto v = env.value(QStringLiteral("TELEDEEP_API"));
	if (!v.isEmpty()) {
		return v.endsWith('/') ? v.chopped(1) : v;
	}
	return QStringLiteral("https://api.deepdesignpc.online");
}

QString ApiV1(const QString &path) {
	const auto p = path.startsWith('/') ? path : ('/' + path);
	return ApiBase() + QStringLiteral("/api/v1") + p;
}

QString webSocketUrl(const QString &token) {
	auto url = QUrl(ApiBase());
	const auto scheme = (url.scheme() == QLatin1String("https"))
		? QStringLiteral("wss")
		: QStringLiteral("ws");
	url.setScheme(scheme);
	url.setPath(QStringLiteral("/ws"));
	QUrlQuery query;
	query.addQueryItem(QStringLiteral("token"), token);
	query.addQueryItem(QStringLiteral("clientId"), clientId());
	url.setQuery(query);
	return url.toString();
}

} // namespace Deep
