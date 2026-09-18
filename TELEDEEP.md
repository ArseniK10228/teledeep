# TeleDeep

**Telegram Desktop UI** + **Deep Messenger** (`api.deepdesignpc.online`), вход по email.

| | |
|---|---|
| Сайт (GitHub Pages) | https://arsenik10228.github.io/teledeep/ |
| Сборки | https://github.com/ArseniK10228/teledeep/releases/tag/teledeep-downloads |
| Deep Messenger (готовые .exe / .apk) | https://github.com/ArseniK10228/deep-messenger/releases/tag/downloads |
| Документация | [docs/TELEDEEP.md](docs/TELEDEEP.md) |

```bash
git remote rename origin upstream   # если origin = telegramdesktop/tdesktop
git remote add origin git@github.com:ArseniK10228/teledeep.git
git push -u origin dev
```

Сборка: `-DTELEDEEP_BUILD=ON` — см. [docs/TELEDEEP.md](docs/TELEDEEP.md).
