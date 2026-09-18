#include "deep/deep_session.h"

namespace Deep {

Session &Session::instance() {
	static Session inst;
	return inst;
}

bool Session::hasToken() const {
	return !_token.isEmpty();
}

QString Session::token() const {
	return _token;
}

void Session::setToken(const QString &token) {
	_token = token;
}

void Session::clear() {
	_token.clear();
	_userId.clear();
	_peerToConversation.clear();
	_conversationToPeer.clear();
	_msgToUuid.clear();
	_uuidToMsg.clear();
	_lastEventByConversation.clear();
}

QString Session::userId() const {
	return _userId;
}

void Session::setUserId(const QString &id) {
	_userId = id;
}

void Session::bindPeerConversation(PeerId peer, const QString &conversationId) {
	_peerToConversation.insert(peer, conversationId);
	_conversationToPeer.insert(conversationId, peer);
}

QString Session::conversationForPeer(PeerId peer) const {
	return _peerToConversation.value(peer);
}

PeerId Session::peerForConversation(const QString &conversationId) const {
	return _conversationToPeer.value(conversationId);
}

void Session::registerMessageUuid(MsgId localId, const QString &uuid) {
	_msgToUuid.insert(localId, uuid);
	_uuidToMsg.insert(uuid, localId);
}

QString Session::messageUuid(MsgId localId) const {
	return _msgToUuid.value(localId);
}

MsgId Session::messageIdForUuid(const QString &uuid) const {
	return _uuidToMsg.value(uuid);
}

void Session::setLastEventMessage(
		const QString &conversationId,
		const QString &messageId) {
	_lastEventByConversation.insert(conversationId, messageId);
}

QString Session::lastEventMessage(const QString &conversationId) const {
	return _lastEventByConversation.value(conversationId);
}

} // namespace Deep
