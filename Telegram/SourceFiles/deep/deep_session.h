#pragma once

#include "data/data_peer_id.h"

#include <QString>
#include <QHash>

namespace Deep {

class Session {
public:
	static Session &instance();

	[[nodiscard]] bool hasToken() const;
	[[nodiscard]] QString token() const;
	void setToken(const QString &token);
	void clear();

	[[nodiscard]] QString userId() const;
	void setUserId(const QString &id);

	void bindPeerConversation(PeerId peer, const QString &conversationId);
	[[nodiscard]] QString conversationForPeer(PeerId peer) const;
	[[nodiscard]] PeerId peerForConversation(const QString &conversationId) const;

	void registerMessageUuid(MsgId localId, const QString &uuid);
	[[nodiscard]] QString messageUuid(MsgId localId) const;
	[[nodiscard]] MsgId messageIdForUuid(const QString &uuid) const;

	void setLastEventMessage(const QString &conversationId, const QString &messageId);
	[[nodiscard]] QString lastEventMessage(const QString &conversationId) const;

private:
	QString _token;
	QString _userId;
	QHash<PeerId, QString> _peerToConversation;
	QHash<QString, PeerId> _conversationToPeer;
	QHash<MsgId, QString> _msgToUuid;
	QHash<QString, MsgId> _uuidToMsg;
	QHash<QString, QString> _lastEventByConversation;
};

} // namespace Deep
