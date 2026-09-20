#pragma once

#include "data/data_messages.h"

#include <functional>

class ApiWrap;
class History;

namespace Deep {

void requestHistory(
	ApiWrap *api,
	not_null<History*> history,
	MsgId messageId,
	Data::LoadDirection slice,
	std::function<void()> done);

} // namespace Deep
