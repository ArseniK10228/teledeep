#pragma once

namespace Main {
class Session;
} // namespace Main

namespace Deep {

void realtimeAttach(not_null<Main::Session*> session);
void realtimeDetach();

} // namespace Deep
