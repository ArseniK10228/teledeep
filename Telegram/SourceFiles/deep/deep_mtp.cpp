#include "deep/deep_mtp.h"

#include "data/data_peer_id.h"
#include "scheme.h"

#include <QUuid>

namespace Deep {
namespace {

[[nodiscard]] QString pickName(
		const QString &displayName,
		const QString &email) {
	if (!displayName.isEmpty()) {
		return displayName;
	}
	return email;
}

} // namespace

uint64 bareIdFromUuid(const QString &uuid) {
	const auto quuid = QUuid(uuid);
	if (!quuid.isNull()) {
		const auto bytes = quuid.toRfc4122();
		uint64 value = 0;
		for (int i = 0; i != 8; ++i) {
			value = (value << 8) | uchar(bytes[i]);
		}
		value &= 0x7FFFFFFFFFFFFFFFULL;
		if (value < 10) {
			value |= 0x100000000ULL;
		}
		return value;
	}
	return uint64(qHash(uuid) & 0x7FFFFFFFFFFFFFFFULL) | 1ULL;
}

PeerId peerFromDeepUser(const QString &uuid) {
	return peerFromUser(UserId(bareIdFromUuid(uuid)));
}

MsgId messageIdFromDeep(const QString &messageUuid) {
	const auto h = uint32(qHash(messageUuid));
	const auto id = int(h & 0x7FFFFFFF);
	return MsgId(id > 0 ? id : 1);
}

MTPUser makeUser(
		const QString &uuid,
		const QString &displayName,
		bool self) {
	const auto flags = MTPDuser::Flag::f_first_name
		| (self ? MTPDuser::Flag::f_self : MTPDuser::Flag());
	return MTP_user(
		MTP_flags(flags),
		MTP_long(bareIdFromUuid(uuid)),
		MTP_long(0),
		MTP_string(pickName(displayName, uuid)),
		MTPstring(),
		MTPstring(),
		MTPstring(),
		MTPUserProfilePhoto(),
		MTPUserStatus(),
		MTPint(),
		MTPVector<MTPRestrictionReason>(),
		MTPstring(),
		MTPstring(),
		MTPEmojiStatus(),
		MTPVector<MTPUsername>(),
		MTPRecentStory(),
		MTPPeerColor(),
		MTPPeerColor(),
		MTPint(),
		MTPlong(),
		MTPlong(),
		MTPlong());
}

MTPMessage makeTextMessage(
		MsgId id,
		PeerId peerId,
		PeerId fromPeerId,
		TimeId date,
		const QString &text,
		bool outgoing) {
	const auto flags = (outgoing ? MTPDmessage::Flag::f_out : MTPDmessage::Flag())
		| (fromPeerId ? MTPDmessage::Flag::f_from_id : MTPDmessage::Flag());
	return MTP_message(
		MTP_flags(flags),
		MTP_int(id.bare),
		fromPeerId ? peerToMTP(fromPeerId) : MTPPeer(),
		MTPint(), // from_boosts_applied
		MTPstring(), // from_rank
		peerToMTP(peerId),
		MTPPeer(), // saved_peer_id
		MTPMessageFwdHeader(),
		MTPlong(), // via_bot_id
		MTPlong(), // via_business_bot_id
		MTPPeer(), // guestchat_via_from
		MTPMessageReplyHeader(),
		MTP_int(date),
		MTP_string(text),
		MTP_messageMediaEmpty(),
		MTPReplyMarkup(),
		MTPVector<MTPMessageEntity>(),
		MTPint(), // views
		MTPint(), // forwards
		MTPMessageReplies(),
		MTPint(), // edit_date
		MTPstring(), // post_author
		MTPlong(), // grouped_id
		MTPMessageReactions(),
		MTPVector<MTPRestrictionReason>(),
		MTPint(), // ttl_period
		MTPint(), // quick_reply_shortcut_id
		MTPlong(), // effect
		MTPFactCheck(),
		MTPint(), // report_delivery_until_date
		MTPlong(), // paid_message_stars
		MTPSuggestedPost(),
		MTPint(), // schedule_repeat_period
		MTPstring(), // summary_from_language
		MTPRichMessage());
}

MTPDialog makeDialog(PeerId peerId, MsgId topMessageId) {
	const auto top = topMessageId.bare;
	return MTP_dialog(
		MTP_flags(0),
		peerToMTP(peerId),
		MTP_int(top),
		MTP_int(top),
		MTP_int(top),
		MTP_int(0),
		MTP_int(0),
		MTP_int(0),
		MTP_int(0),
		MTP_peerNotifySettings(
			MTP_flags(0),
			MTPBool(),
			MTPBool(),
			MTPint(),
			MTPNotificationSound(),
			MTPNotificationSound(),
			MTPNotificationSound(),
			MTPBool(),
			MTPBool(),
			MTPNotificationSound(),
			MTPNotificationSound(),
			MTPNotificationSound()),
		MTPint(),
		MTPDraftMessage(),
		MTPint(),
		MTPint());
}

} // namespace Deep
