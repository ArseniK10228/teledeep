#pragma once

#include "data/data_peer_id.h"

#include <QString>

class MTPUser;
class MTPMessage;
class MTPDialog;

namespace Deep {

[[nodiscard]] uint64 bareIdFromUuid(const QString &uuid);
[[nodiscard]] PeerId peerFromDeepUser(const QString &uuid);
[[nodiscard]] MsgId messageIdFromDeep(const QString &messageUuid);

[[nodiscard]] MTPUser makeUser(
	const QString &uuid,
	const QString &displayName,
	bool self);
[[nodiscard]] MTPMessage makeTextMessage(
	MsgId id,
	PeerId peerId,
	PeerId fromPeerId,
	TimeId date,
	const QString &text,
	bool outgoing);
[[nodiscard]] MTPDialog makeDialog(PeerId peerId, MsgId topMessageId);

} // namespace Deep
