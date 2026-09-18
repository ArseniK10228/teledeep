# TeleDeep

**GitHub:** [ArseniK10228/teledeep](https://github.com/ArseniK10228/teledeep)  
**Страница:** https://arsenik10228.github.io/teledeep/  
**Releases:** https://github.com/ArseniK10228/teledeep/releases/tag/teledeep-downloads  
**Готовый клиент (пока Deep Messenger):** https://github.com/ArseniK10228/deep-messenger/releases/tag/downloads

Это **не самодельный UI**. Здесь лежит **оригинальный исходник [Telegram Desktop](https://github.com/telegramdesktop/tdesktop)** (GPLv3).

`git remote -v` → `origin` указывает на `telegramdesktop/tdesktop`. Для своего форка:

```bash
git remote rename origin upstream
git remote add origin <твой-репозиторий>
git push -u origin master
```

## Сборка с Deep (email, без телефона/QR)

В CMake:

```text
-DTELEDEEP_BUILD=ON
```

Опционально база API: env `TELEDEEP_API=https://api.deepdesignpc.online`.

Старт → email → код → сессия; список чатов тянется с `GET /api/v1/conversations`. Реализовано в коде: история (`GET .../messages`), отправка текста/файлов, поллинг новых сообщений (~2.5 с). Звонки — пока через штатный UI Telegram (не Deep).

**Сборка установщика TeleDeep** — только на машине с [TBuild + VS + Qt](building-win.md):

```bat
cmake -B out -DTELEDEEP_BUILD=ON -DTDESKTOP_API_ID=... -DTDESKTOP_API_HASH=...
cmake --build out --config Release --target Telegram
```

Готовый `.exe` установщик upstream собирается через `Packer` (см. building-win.md), не из этого репо без TBuild.

## Собрать 1 в 1 как Telegram (Windows)

Официально: [docs/building-win.md](./building-win.md).

Кратко:

1. Папка сборки, например `D:\TBuild` → `ThirdParty`, `Libraries`.
2. Python 3.10, Git, **Visual Studio 2026** + SDK **10.0.26100.0**, Qt по инструкции из building-win.
3. `api_id` / `api_hash` с [my.telegram.org](https://my.telegram.org) — **без них клиент к серверам Telegram не стартует** (пока не вырежешь MTProto).
4. В `vcvars64` (см. building-win): `configure.bat` / `cmake` — как в upstream README.

Пока не соберёшь upstream — это **тот же Telegram Desktop**, не «почти».

## Почему «просто подставить Deep API» — не одна правка

Клиент заточен под **MTProto** + сотни методов `MTP*` (`ApiWrap`, `Main::Session::mtp()`).

Твой Deep — **REST + WebSocket + JWT**, другая модель сообщений/медиа/звонков.

Варианты (реально):

| Путь | Суть | Объём |
|------|------|--------|
| **A. MTProto-шлюз** | Сервер говорит MTProto с клиентом, внутри — Deep API | Очень большой (почти писать свой Telegram DC) |
| **B. Форк клиента** | Подменить слой `MTP::Instance` / `ApiWrap` на HTTP+WS к Deep | Большой, но контролируемый (месяцы, не «вечер») |
| **C. Оставить TG DC** | Форк только для UI/форков фич | Deep не подключается |

Структура, с которой начинать копать в коде:

- `Telegram/SourceFiles/main/main_session.{h,cpp}` — сессия, `mtp()`, `api()`
- `Telegram/SourceFiles/apiwrap.{h,cpp}` — обёртка над MTP-запросами
- `Telegram/SourceFiles/mtproto/` — транспорт, DC, шифрование
- `Telegram/SourceFiles/storage/file_upload.{h,cpp}` — загрузка файлов (аналог твоего `/upload`)

План интеграции Deep (фаза B): [DEEP-BACKEND-PLAN.md](./DEEP-BACKEND-PLAN.md).

## Лицензия

GPLv3. Распространяешь бинарник — обязанности GPL (исходники, та же лицензия на производное). Юрист, не я.

## VPN

«Без VPN» — это **DNS + доступность `api.deepdesignpc.online`**, не клиент. tdesktop по умолчанию ходит на **серверы Telegram**; после форка под Deep — на твой хост (как в `deep-messenger`).
