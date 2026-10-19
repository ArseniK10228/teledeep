#pragma once

#include "api/api_common.h"

class ApiWrap;
enum class SendMediaType;
struct SendingAlbum;

namespace Ui {
struct PreparedList;
} // namespace Ui

namespace Deep {

void sendTextMessage(
	ApiWrap *api,
	Api::MessageToSend &&message,
	std::optional<MsgId> localMessageId);

void sendFiles(
	ApiWrap *api,
	Ui::PreparedList &&list,
	SendMediaType type,
	std::shared_ptr<SendingAlbum> album,
	Api::SendAction action);

} // namespace Deep
