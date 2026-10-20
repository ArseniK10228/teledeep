#pragma once

namespace Data {
class Folder;
} // namespace Data

class ApiWrap;

namespace Deep {

void finishDialogLoad(ApiWrap *api, Data::Folder *folder);
void requestDialogs(ApiWrap *api, Data::Folder *folder);

} // namespace Deep
