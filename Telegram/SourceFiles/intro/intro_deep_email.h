#pragma once

#include "intro/intro_step.h"

namespace Ui {
class InputField;
} // namespace Ui

namespace Intro {
namespace details {

class DeepEmailWidget final : public Step {
public:
	DeepEmailWidget(
		QWidget *parent,
		not_null<Main::Account*> account,
		not_null<Data*> data);

	void setInnerFocus() override;
	void submit() override;
	void resizeEvent(QResizeEvent *e) override;
	bool hasBack() const override {
		return false;
	}

private:
	object_ptr<Ui::InputField> _email;
	bool _waiting = false;

};

} // namespace details
} // namespace Intro
