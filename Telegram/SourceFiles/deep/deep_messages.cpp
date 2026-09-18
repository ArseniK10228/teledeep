#include "deep/deep_messages.h"

#include "data/data_session.h"
#include "deep/deep_mtp.h"
#include "deep/deep_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"
#include "base/unixtime.h"

#include <QDateTime>

namespace Deep {
namespace {

TimeId parseTime(const QJsonValue &v) {
	const auto s = v.toString();
	if (s.isEmpty()) {
		return base::unixtime::now();
	}
	const auto dt = QDateTime::fromString(s, Qt::ISODateWithMs);
	if (dt.isValid()) {
		return TimeId(dt.toSecsSinceEpoch());
	}
	const auto dt2 = QDateTime::fromString(s, Qt::ISODate);
	return dt2.isValid() ? TimeId(dt2.toSecsSinceEpoch()) : base::unixtime::now();
}

} // namespace

QString messagePreview(const QJsonObject &msg) {
	const auto body = msg.value(QStringLiteral("body")).toString();
	if (!body.isEmpty()) {
		return body;
	}
	const auto kind = msg.value(QStringLiteral("kind")).toString();
	if (kind == QStringLiteral("image")) {
		return QStringLiteral("📷 Photo");
	}
	if (kind == QStringLiteral("file")) {
		return QStringLiteral("📎 File");
	}
	if (kind == QStringLiteral("voice")) {
		return QStringLiteral("🎤 Voice");
	}
	if (kind == QStringLiteral("video_note")) {
		return QStringLiteral("📹 Video message");
	}
	return QString();
}

HistoryItem *applyMessageJson(
		not_null<Main::Session*> session,
		PeerId peerId,
		const QJsonObject &msg,
		NewMessageType type) {
	const auto uuid = msg.value(QStringLiteral("id")).toString();
	if (uuid.isEmpty()) {
		return nullptr;
	}
	const auto msgId = messageIdFromDeep(uuid);
	const auto existing = session->data().message({ peerId, msgId });
	if (existing) {
		Session::instance().registerMessageUuid(msgId, uuid);
		return existing;
	}
	const auto selfId = Session::instance().userId();
	auto senderUuid = msg.value(QStringLiteral("senderId")).toString();
	if (senderUuid.isEmpty()) {
		senderUuid = msg.value(QStringLiteral("sender_id")).toString();
	}
	if (senderUuid.isEmpty()) {
		return nullptr;
	}
	const auto outgoing = (senderUuid == selfId);
	const auto fromPeer = outgoing ? session->userPeerId() : peerId;
	const auto item = session->data().addNewMessage(
		makeTextMessage(
			msgId,
			peerId,
			fromPeer,
			parseTime(msg.value(QStringLiteral("createdAt")).isUndefined()
				? msg.value(QStringLiteral("created_at"))
				: msg.value(QStringLiteral("createdAt"))),
			messagePreview(msg),
			outgoing),
		MessageFlags(),
		type);
	Session::instance().registerMessageUuid(msgId, uuid);
	return item;
}

void applyIncomingMessage(
		not_null<Main::Session*> session,
		const QJsonObject &msg) {
	const auto convId = msg.value(QStringLiteral("conversationId")).toString();
	auto peerId = Session::instance().peerForConversation(convId);
	if (!peerId) {
		return;
	}
	const auto item = applyMessageJson(
		session,
		peerId,
		msg,
		NewMessageType::Unread);
	if (!item) {
		return;
	}
	Session::instance().setLastEventMessage(convId, msg.value(QStringLiteral("id")).toString());
	session->data().sendHistoryChangeNotifications();
}

} // namespace Deep
