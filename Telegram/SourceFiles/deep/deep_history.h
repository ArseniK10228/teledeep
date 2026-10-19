#pragma once

#include "data/data_msg_id.h"

#include <functional>

class ApiWrap;
class History;

namespace Data {
enum class LoadDirection : char;
} // namespace Data

namespace Deep {

void requestHistory(
	ApiWrap *api,
	not_null<History*> history,
	MsgId messageId,
	Data::LoadDirection slice,
	std::function<void()> done);

} // namespace Deep
