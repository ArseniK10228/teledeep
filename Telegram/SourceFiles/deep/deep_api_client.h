#pragma once

#include <functional>

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>

namespace Deep {

using ApiDone = std::function<void(const QJsonObject &)>;
using ApiFail = std::function<void(const QString &)>;
using ApiArrayDone = std::function<void(const QJsonArray &)>;

void sendEmailCode(const QString &email, ApiDone done, ApiFail fail);
void verifyEmailCode(
	const QString &requestId,
	const QString &code,
	ApiDone done,
	ApiFail fail);
void fetchMe(ApiDone done, ApiFail fail);
void fetchConversations(ApiDone done, ApiFail fail);
void fetchMessages(
	const QString &conversationId,
	const QString &beforeMessageId,
	int limit,
	ApiDone done,
	ApiFail fail);
void postTextMessage(
	const QString &conversationId,
	const QString &body,
	ApiDone done,
	ApiFail fail);
void uploadFile(
	const QString &conversationId,
	const QString &filepath,
	const QString &caption,
	ApiDone done,
	ApiFail fail);
void markConversationRead(const QString &conversationId);

} // namespace Deep
