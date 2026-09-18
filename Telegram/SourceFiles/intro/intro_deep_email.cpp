#include "intro/intro_deep_email.h"

#include "intro/intro_deep_code.h"
#include "deep/deep_api_client.h"
#include "lang/lang_keys.h"
#include "ui/widgets/fields/input_field.h"
#include "styles/style_intro.h"

#include <QJsonObject>

namespace Intro {
namespace details {

DeepEmailWidget::DeepEmailWidget(
	QWidget *parent,
	not_null<Main::Account*> account,
	not_null<Data*> data)
: Step(parent, account, data)
, _email(this, st::introName, tr::lng_settings_cloud_login_email_placeholder()) {
	_email->setInputMethodHints(
		Qt::ImhEmailCharactersOnly
		| Qt::ImhNoAutoUppercase
		| Qt::ImhNoPredictiveText);
	connect(_email, &Ui::InputField::submitted, [=] { submit(); });
	connect(_email, &Ui::InputField::changed, [=] { hideError(); });

	setTitleText(tr::lng_intro_email_setup_title());
	setDescriptionText(tr::lng_settings_cloud_login_email_about());
	setErrorCentered(true);
}

void DeepEmailWidget::setInnerFocus() {
	_email->setFocusFast();
}

void DeepEmailWidget::resizeEvent(QResizeEvent *e) {
	Step::resizeEvent(e);
	_email->moveToLeft(
		contentLeft(),
		contentTop() + st::introStepFieldTop);
}

void DeepEmailWidget::submit() {
	if (_waiting) {
		return;
	}
	const auto email = _email->getLastText().trimmed();
	if (email.isEmpty()) {
		_email->showError();
		_email->setFocus();
		return;
	}
	_waiting = true;
	hideError();
	Deep::sendEmailCode(
		email,
		[=](const QJsonObject &result) {
			_waiting = false;
			getData()->email = result.value(QStringLiteral("email")).toString(email);
			getData()->deepAuthRequestId = result.value(
				QStringLiteral("requestId")).toString();
			getData()->codeLength = 6;
			goNext<DeepCodeWidget>();
		},
		[=](const QString &error) {
			_waiting = false;
			_email->setFocus();
			_email->showError();
			showError(error);
		});
}

} // namespace details
} // namespace Intro
