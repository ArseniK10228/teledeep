#pragma once

#include "api/api_common.h"

class ApiWrap;

namespace Ui {
struct PreparedList;
} // namespace Ui

enum class SendMediaType;
struct SendAction;
struct SendingAlbum;

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
	SendAction action);

} // namespace Deep
