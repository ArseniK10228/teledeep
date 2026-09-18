#include "deep/deep_api_client.h"

#include "deep/deep_config.h"
#include "deep/deep_session.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <memory>

namespace Deep {
namespace {

void jsonRequest(
	const QString &method,
	const QString &path,
	const QByteArray &body,
	bool auth,
	ApiDone done,
	ApiFail fail) {
	auto *nam = new QNetworkAccessManager();
	const auto url = QUrl(ApiBase() + path);
	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	if (auth && Session::instance().hasToken()) {
		req.setRawHeader(
			"Authorization",
			("Bearer " + Session::instance().token()).toUtf8());
	}
	auto *reply = (method == QLatin1String("POST"))
		? nam->post(req, body)
		: nam->get(req);
	QObject::connect(reply, &QNetworkReply::finished, [=]() {
		const auto guard = std::unique_ptr<QNetworkAccessManager>(nam);
		const auto data = reply->readAll();
		const auto status = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (reply->error() != QNetworkReply::NoError || status >= 400) {
			auto message = reply->errorString();
			const auto doc = QJsonDocument::fromJson(data);
			if (doc.isObject()) {
				const auto err = doc.object().value(QStringLiteral("error"));
				if (err.isString() && !err.toString().isEmpty()) {
					message = err.toString();
				}
			}
			fail(message);
			reply->deleteLater();
			return;
		}
		const auto doc = QJsonDocument::fromJson(data);
		if (!doc.isObject()) {
			fail(QStringLiteral("invalid json"));
		} else {
			done(doc.object());
		}
		reply->deleteLater();
	});
}

} // namespace

void sendEmailCode(const QString &email, ApiDone done, ApiFail fail) {
	QJsonObject o;
	o.insert(QStringLiteral("email"), email);
	jsonRequest(
		QStringLiteral("POST"),
		QStringLiteral("/api/v1/auth/email/send"),
		QJsonDocument(o).toJson(QJsonDocument::Compact),
		false,
		done,
		fail);
}

void verifyEmailCode(
		const QString &requestId,
		const QString &code,
		ApiDone done,
		ApiFail fail) {
	QJsonObject o;
	o.insert(QStringLiteral("requestId"), requestId);
	o.insert(QStringLiteral("code"), code);
	jsonRequest(
		QStringLiteral("POST"),
		QStringLiteral("/api/v1/auth/email/verify"),
		QJsonDocument(o).toJson(QJsonDocument::Compact),
		false,
		done,
		fail);
}

void fetchMe(ApiDone done, ApiFail fail) {
	jsonRequest(
		QStringLiteral("GET"),
		QStringLiteral("/api/v1/me"),
		QByteArray(),
		true,
		done,
		fail);
}

void fetchConversations(ApiDone done, ApiFail fail) {
	jsonRequest(
		QStringLiteral("GET"),
		QStringLiteral("/api/v1/conversations"),
		QByteArray(),
		true,
		done,
		fail);
}

void fetchMessages(
		const QString &conversationId,
		const QString &beforeMessageId,
		int limit,
		ApiDone done,
		ApiFail fail) {
	auto path = QStringLiteral("/api/v1/conversations/")
		+ conversationId
		+ QStringLiteral("/messages?limit=")
		+ QString::number(limit);
	if (!beforeMessageId.isEmpty()) {
		path += QStringLiteral("&before=") + beforeMessageId;
	}
	jsonRequest(
		QStringLiteral("GET"),
		path,
		QByteArray(),
		true,
		done,
		fail);
}

void postTextMessage(
		const QString &conversationId,
		const QString &body,
		ApiDone done,
		ApiFail fail) {
	QJsonObject o;
	o.insert(QStringLiteral("kind"), QStringLiteral("text"));
	o.insert(QStringLiteral("body"), body);
	const auto path = QStringLiteral("/api/v1/conversations/")
		+ conversationId
		+ QStringLiteral("/messages");
	jsonRequest(
		QStringLiteral("POST"),
		path,
		QJsonDocument(o).toJson(QJsonDocument::Compact),
		true,
		done,
		fail);
}

void uploadFile(
		const QString &conversationId,
		const QString &filepath,
		const QString &caption,
		ApiDone done,
		ApiFail fail) {
	auto *nam = new QNetworkAccessManager();
	const auto url = QUrl(ApiV1(
		QStringLiteral("/conversations/") + conversationId + QStringLiteral("/upload")));
	QNetworkRequest req(url);
	if (Session::instance().hasToken()) {
		req.setRawHeader(
			"Authorization",
			("Bearer " + Session::instance().token()).toUtf8());
	}
	auto multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	const auto file = std::make_shared<QFile>(filepath);
	if (!file->open(QIODevice::ReadOnly)) {
		fail(QStringLiteral("cannot read file"));
		delete multi;
		delete nam;
		return;
	}
	const auto info = QFileInfo(filepath);
	QHttpPart filePart;
	filePart.setHeader(
		QNetworkRequest::ContentDispositionHeader,
		QStringLiteral("form-data; name=\"file\"; filename=\"")
			+ info.fileName()
			+ QLatin1Char('"'));
	filePart.setBodyDevice(file.get());
	multi->append(filePart);
	if (!caption.isEmpty()) {
		QHttpPart capPart;
		capPart.setHeader(
			QNetworkRequest::ContentDispositionHeader,
			QStringLiteral("form-data; name=\"caption\""));
		capPart.setBody(caption.toUtf8());
		multi->append(capPart);
	}
	auto *reply = nam->post(req, multi);
	multi->setParent(reply);
	QObject::connect(reply, &QNetworkReply::finished, [=, file]() {
		const auto guard = std::unique_ptr<QNetworkAccessManager>(nam);
		const auto data = reply->readAll();
		const auto status = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (reply->error() != QNetworkReply::NoError || status >= 400) {
			auto message = reply->errorString();
			const auto doc = QJsonDocument::fromJson(data);
			if (doc.isObject()) {
				const auto err = doc.object().value(QStringLiteral("error"));
				if (err.isString()) {
					message = err.toString();
				}
			}
			fail(message);
		} else {
			const auto doc = QJsonDocument::fromJson(data);
			if (doc.isObject()) {
				done(doc.object());
			} else {
				fail(QStringLiteral("invalid json"));
			}
		}
		reply->deleteLater();
	});
}

void markConversationRead(const QString &conversationId) {
	jsonRequest(
		QStringLiteral("POST"),
		QStringLiteral("/api/v1/conversations/") + conversationId + QStringLiteral("/read"),
		QByteArray(),
		true,
		[](const QJsonObject &) {},
		[](const QString &) {});
}

} // namespace Deep
