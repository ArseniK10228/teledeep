#pragma once

#include "intro/intro_step.h"

namespace Ui {
class CodeInput;
} // namespace Ui

namespace Intro {
namespace details {

class DeepCodeWidget final : public Step {
public:
	DeepCodeWidget(
		QWidget *parent,
		not_null<Main::Account*> account,
		not_null<Data*> data);

	void setInnerFocus() override;
	void submit() override;
	void resizeEvent(QResizeEvent *e) override;
	bool hasBack() const override {
		return true;
	}

private:
	void submitCode(const QString &code);

	object_ptr<Ui::CodeInput> _code;
	bool _waiting = false;

};

} // namespace details
} // namespace Intro
