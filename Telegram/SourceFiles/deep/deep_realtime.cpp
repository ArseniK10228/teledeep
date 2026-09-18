#include "deep/deep_realtime.h"

#include "deep/deep_api_client.h"
#include "deep/deep_messages.h"
#include "deep/deep_session.h"
#include "main/main_session.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>
#include <QTimer>

namespace Deep {
namespace {

Main::Session *ActiveSession = nullptr;
QTimer *PollTimer = nullptr;

QString lastMessageId(const QJsonObject &conv) {
	const auto last = conv.value(QStringLiteral("last_message")).toObject();
	if (last.isEmpty()) {
		return QString();
	}
	return last.value(QStringLiteral("id")).toString();
}

void pollOnce() {
	const auto session = ActiveSession;
	if (!session || !Session::instance().hasToken()) {
		return;
	}
	fetchConversations(
		[=](const QJsonObject &root) {
			const auto list = root.value(QStringLiteral("conversations")).toArray();
			for (const auto &entry : list) {
				const auto conv = entry.toObject();
				const auto convId = conv.value(QStringLiteral("id")).toString();
				const auto msgId = lastMessageId(conv);
				if (convId.isEmpty() || msgId.isEmpty()) {
					continue;
				}
				if (Session::instance().lastEventMessage(convId) == msgId) {
					continue;
				}
				const auto peerId = Session::instance().peerForConversation(convId);
				if (!peerId) {
					continue;
				}
				const auto last = conv.value(QStringLiteral("last_message")).toObject();
				applyMessageJson(
					session,
					peerId,
					last,
					NewMessageType::Unread);
				Session::instance().setLastEventMessage(convId, msgId);
			}
			session->data().sendHistoryChangeNotifications();
		},
		[](const QString &) {});
}

void ensureTimer() {
	if (PollTimer) {
		return;
	}
	PollTimer = new QTimer();
	PollTimer->setInterval(2500);
	QObject::connect(PollTimer, &QTimer::timeout, [] { pollOnce(); });
	PollTimer->start();
}

} // namespace

void realtimeAttach(not_null<Main::Session*> session) {
	ActiveSession = session;
	ensureTimer();
	pollOnce();
}

void realtimeDetach() {
	ActiveSession = nullptr;
}

} // namespace Deep
