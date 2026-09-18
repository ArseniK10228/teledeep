#include "deep/deep_client_id.h"

#include <QSettings>
#include <QUuid>

namespace Deep {

QString clientId() {
	auto settings = QSettings(
		QStringLiteral("TeleDeep"),
		QStringLiteral("TeleDeep"));
	const auto key = QStringLiteral("clientId");
	auto id = settings.value(key).toString();
	if (id.isEmpty()) {
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
		settings.setValue(key, id);
	}
	return id;
}

} // namespace Deep
