#include "intro/intro_deep_code.h"

#include "intro/intro_code_input.h"
#include "intro/intro_widget.h"
#include "deep/deep_api_client.h"
#include "deep/deep_mtp.h"
#include "deep/deep_session.h"
#include "lang/lang_keys.h"
#include "ui/text/format_values.h"
#include "ui/text/text_utilities.h"
#include "styles/style_intro.h"

#include <QJsonObject>

namespace Intro {
namespace details {

DeepCodeWidget::DeepCodeWidget(
	QWidget *parent,
	not_null<Main::Account*> account,
	not_null<Data*> data)
: Step(parent, account, data)
, _code(this) {
	_code->setDigitsCountMax(getData()->codeLength);
	_code->codeCollected(
	) | rpl::on_next([=](const QString &code) {
		submitCode(code);
	}, lifetime());

	setTitleText(tr::lng_intro_fragment_title());
	setDescriptionText(tr::lng_intro_email_confirm_subtitle(
		lt_email,
		rpl::single(Ui::Text::WrapEmailPattern(getData()->email)),
		tr::marked));
	setErrorCentered(true);
}

void DeepCodeWidget::setInnerFocus() {
	_code->setFocus();
}

void DeepCodeWidget::resizeEvent(QResizeEvent *e) {
	Step::resizeEvent(e);
	_code->moveToLeft(
		contentLeft(),
		contentTop() + st::introStepFieldTop);
}

void DeepCodeWidget::submit() {
	_code->requestCode();
}

void DeepCodeWidget::submitCode(const QString &code) {
	if (_waiting) {
		return;
	}
	const auto requestId = getData()->deepAuthRequestId;
	if (requestId.isEmpty()) {
		showError(rpl::single(QStringLiteral("session expired")));
		return;
	}
	_waiting = true;
	hideError();
	Deep::verifyEmailCode(
		requestId,
		code,
		[=](const QJsonObject &result) {
			_waiting = false;
			const auto token = result.value(QStringLiteral("token")).toString();
			const auto user = result.value(QStringLiteral("user")).toObject();
			Deep::Session::instance().setToken(token);
			Deep::Session::instance().setUserId(
				user.value(QStringLiteral("id")).toString());
			const auto name = user.value(QStringLiteral("displayName")).toString();
			const auto email = user.value(QStringLiteral("email")).toString();
			const auto display = !name.isEmpty() ? name : email;
			finish(Deep::makeUser(
				user.value(QStringLiteral("id")).toString(),
				display,
				true), QImage());
		},
		[=](const QString &error) {
			_waiting = false;
			_code->showError();
			showError(rpl::single(error));
		});
}

} // namespace details
} // namespace Intro
