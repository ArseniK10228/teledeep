#include "deep/deep_dialogs_loader.h"

#include "apiwrap.h"
#include "data/data_folder.h"
#include "data/data_session.h"
#include "deep/deep_api_client.h"
#include "deep/deep_mtp.h"
#include "deep/deep_session.h"
#include "main/main_session.h"
#include "base/unixtime.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

namespace Deep {
namespace {

[[nodiscard]] TimeId parseTime(const QJsonValue &v) {
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

[[nodiscard]] QString previewBody(const QJsonObject &last) {
	const auto body = last.value(QStringLiteral("body")).toString();
	if (!body.isEmpty()) {
		return body;
	}
	const auto kind = last.value(QStringLiteral("kind")).toString();
	if (kind == QStringLiteral("image")) {
		return QStringLiteral("📷 Photo");
	}
	if (kind == QStringLiteral("file")) {
		return QStringLiteral("📎 File");
	}
	return QString();
}

void finishDialogLoad(ApiWrap *api, Data::Folder *folder) {
	const auto state = api->dialogsLoadState(folder);
	if (!state) {
		return;
	}
	state->requestId = 0;
	state->listReceived = true;
	state->pinnedReceived = true;
	state->pinnedRequestId = 0;
	api->dialogsLoadFinish(folder);
	api->requestMoreDialogsIfNeeded();
	api->session().data().chatsListChanged(folder);
}

} // namespace

void requestDialogs(ApiWrap *api, Data::Folder *folder) {
	const auto state = api->dialogsLoadState(folder);
	if (!state || state->requestId || state->listReceived) {
		return;
	}
	state->requestId = 1;

	fetchConversations(
		[=](const QJsonObject &root) {
			const auto selfId = Session::instance().userId();
			const auto &session = api->session();
			QVector<MTPUser> users;
			QVector<MTPMessage> messages;
			QVector<MTPDialog> dialogs;

			const auto list = root.value(QStringLiteral("conversations")).toArray();
			for (const auto &entry : list) {
				const auto conv = entry.toObject();
				const auto convId = conv.value(QStringLiteral("id")).toString();
				const auto peers = conv.value(QStringLiteral("peers")).toArray();
				if (peers.isEmpty()) {
					continue;
				}
				const auto peerObj = peers.at(0).toObject();
				const auto peerUuid = peerObj.value(QStringLiteral("id")).toString();
				const auto peerId = peerFromDeepUser(peerUuid);
				Session::instance().bindPeerConversation(peerId, convId);
				const auto displayName = peerObj.value(QStringLiteral("displayName")).toString();
				users.push_back(makeUser(peerUuid, displayName, false));

				const auto last = conv.value(QStringLiteral("last_message")).toObject();
				MsgId topId = MsgId(0);
				if (!last.isEmpty()) {
					const auto msgUuid = last.value(QStringLiteral("id")).toString();
					topId = messageIdFromDeep(msgUuid);
					const auto senderId = last.value(QStringLiteral("sender_id")).toString();
					const auto outgoing = (senderId == selfId);
					const auto fromPeer = outgoing
						? session.userPeerId()
						: peerId;
					messages.push_back(makeTextMessage(
						topId,
						peerId,
						fromPeer,
						parseTime(last.value(QStringLiteral("created_at"))),
						previewBody(last),
						outgoing));
				}
				dialogs.push_back(makeDialog(peerId, topId));
			}

			session.data().processUsers(MTP_vector<MTPUser>(users));
			session.data().applyDialogs(
				folder,
				messages,
				dialogs,
				dialogs.size());
			finishDialogLoad(api, folder);
		},
		[=](const QString &) {
			finishDialogLoad(api, folder);
		});
}

} // namespace Deep
