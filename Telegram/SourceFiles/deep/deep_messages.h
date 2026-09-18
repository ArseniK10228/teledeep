#pragma once

#include "data/data_peer_id.h"
#include "history/history.h"

#include <QJsonObject>

class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Deep {

[[nodiscard]] QString messagePreview(const QJsonObject &msg);

[[nodiscard]] HistoryItem *applyMessageJson(
	not_null<Main::Session*> session,
	PeerId peerId,
	const QJsonObject &msg,
	NewMessageType type = NewMessageType::Existing);

void applyIncomingMessage(
	not_null<Main::Session*> session,
	const QJsonObject &msg);

} // namespace Deep
