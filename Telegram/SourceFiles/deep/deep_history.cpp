#include "deep/deep_history.h"

#include "apiwrap.h"
#include "data/data_histories.h"
#include "deep/deep_api_client.h"
#include "deep/deep_messages.h"
#include "deep/deep_session.h"
#include "data/data_history_messages.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

namespace Deep {

void requestHistory(
		ApiWrap *api,
		not_null<History*> history,
		MsgId messageId,
		Data::LoadDirection slice,
		std::function<void()> done) {
	const auto session = &api->session();
	const auto peer = history->peer;
	const auto convId = Session::instance().conversationForPeer(peer->id);
	if (convId.isEmpty()) {
		done();
		return;
	}
	const auto before = messageId
		? Session::instance().messageUuid(messageId)
		: QString();
	fetchMessages(
		convId,
		before,
		80,
		[=](const QJsonObject &root) {
			auto ids = std::vector<MsgId>();
			auto range = MsgRange{ MsgId(0), MsgId(0) };
			const auto arr = root.value(QStringLiteral("messages")).toArray();
			for (const auto &v : arr) {
				const auto item = applyMessageJson(
					session,
					peer->id,
					v.toObject(),
					NewMessageType::Existing);
				if (item) {
					ids.push_back(item->id);
					if (!range.from || item->id < range.from) {
						range.from = item->id;
					}
					if (!range.till || item->id > range.till) {
						range.till = item->id;
					}
				}
			}
			if (!ids.empty()) {
				history->messages().addSlice(std::move(ids), range, int(ids.size()));
			}
			markConversationRead(convId);
			done();
		},
		[=](const QString &) {
			done();
		});
}

} // namespace Deep
