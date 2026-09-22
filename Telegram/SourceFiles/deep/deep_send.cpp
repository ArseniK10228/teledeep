#include "deep/deep_send.h"

#include "apiwrap.h"
#include "base/random.h"
#include "data/data_histories.h"
#include "data/data_session.h"
#include "deep/deep_api_client.h"
#include "deep/deep_messages.h"
#include "deep/deep_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_helpers.h"
#include "main/main_session.h"
#include "ui/chat/attach/attach_prepare.h"
#include "api/api_text_entities.h"
#include "ui/item_text_options.h"
#include "ui/text/text_utilities.h"

namespace Deep {

void sendTextMessage(
		ApiWrap *api,
		Api::MessageToSend &&message,
		std::optional<MsgId> localMessageId) {
	const auto history = message.action.history;
	const auto peer = history->peer;
	const auto convId = Session::instance().conversationForPeer(peer->id);
	if (convId.isEmpty()) {
		return;
	}
	const auto session = &api->session();
	auto action = message.action;
	action.generateLocal = true;
	api->sendAction(action);

	auto sending = TextWithEntities();
	auto left = TextWithEntities {
		message.textWithTags.text,
		TextUtilities::ConvertTextTagsToEntities(message.textWithTags.tags),
	};
	const auto prepareFlags = Ui::ItemTextOptions(
		history,
		not_null<PeerData*>(session->user())).flags;
	TextUtilities::PrepareForSending(left, prepareFlags);
	TextUtilities::Trim(left);
	sending = left;

	const auto newId = FullMsgId(
		peer->id,
		localMessageId
			? localMessageId.value()
			: session->data().nextLocalMessageId());
	const auto randomId = base::RandomValue<uint64>();
	session->data().registerMessageRandomId(randomId, newId);
	session->data().registerMessageSentData(randomId, peer->id, sending.text);

	auto flags = NewMessageFlags(peer);
	const auto local = history->addNewLocalMessage({
		.id = newId.msg,
		.flags = flags,
		.from = NewMessageFromId(action),
		.replyTo = action.replyTo,
		.date = NewMessageDate(action.options),
	}, sending, MTP_messageMediaEmpty());

	const auto body = sending.text;
	postTextMessage(
		convId,
		body,
		[=, session = session, peer = peer, local = local, randomId = randomId, newId = newId](const QJsonObject &root) {
			const auto msg = root.value(QStringLiteral("message")).toObject();
			if (local) {
				local->destroy();
			}
			applyMessageJson(
				session,
				peer->id,
				msg,
				NewMessageType::Last);
			session->data().sendHistoryChangeNotifications();
		},
		[=, session = session, peer = peer, randomId = randomId, newId = newId](const QString &error) {
			api->sendMessageFail(error, peer, randomId, newId);
		});
}

void sendFiles(
		ApiWrap *api,
		Ui::PreparedList &&list,
		SendMediaType type,
		std::shared_ptr<SendingAlbum> album,
		Api::SendAction action) {
	const auto history = action.history;
	const auto peer = history->peer;
	const auto convId = Session::instance().conversationForPeer(peer->id);
	if (convId.isEmpty() || list.files.empty()) {
		return;
	}
	const auto session = &api->session();
	api->sendAction(action);
	for (auto &file : list.files) {
		const auto caption = file.caption.text;
		uploadFile(
			convId,
			file.path,
			caption,
			[=, session = session, peer = peer](const QJsonObject &root) {
				const auto msg = root.value(QStringLiteral("message")).toObject();
				applyMessageJson(
					session,
					peer->id,
					msg,
					NewMessageType::Last);
				session->data().sendHistoryChangeNotifications();
			},
			[=](const QString &) {});
	}
}

} // namespace Deep
