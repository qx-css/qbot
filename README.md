# QdBot

QdBot 是一个使用 C++/WinRT + WinUI 3 编写的 Windows QQ AI 机器人控制端。它通过 OneBot v11 适配器接入 QQ，当前先实现 AI 模式：收到群聊或私聊消息后调用 OpenAI 兼容接口，并把 AI 回复发送回 QQ。

## 功能

- WinUI 3 桌面配置界面。
- OpenAI 兼容 AI API 配置：`base_url`、`api_key`、`model`。
- OneBot v11 HTTP 事件监听和消息发送。
- 支持群聊、私聊开关。
- 支持触发前缀，不填则所有消息都会触发。
- 机器人被拉入群聊后自动发送：`主人，我来咯！！！`
- 预留 `词库` 模式入口，后续可扩展词库匹配。

## 开发环境

1. Visual Studio 2022。
2. 勾选 `使用 C++ 的桌面开发`。
3. 安装 Windows App SDK / WinUI 3 相关组件。
4. 允许 NuGet 还原包。

项目依赖通过 `packages.config` 管理：

- `Microsoft.WindowsAppSDK`
- `Microsoft.Windows.CppWinRT`
- `cpprestsdk.v142`

## 构建运行

1. 打开 `QdBot.sln`。
2. 选择 `x64` 和 `Debug` 或 `Release`。
3. 还原 NuGet 包。
4. 编译并运行 `QdBot`。
5. 在界面填写 AI API 和 OneBot API 配置。
6. 点击 `保存配置`，再点击 `启动`。

配置会保存到程序运行目录下的 `config.json`。

也可以在 PowerShell 里运行：

```powershell
.\build-release.ps1
```

成功后会在 `x64\Release` 相关输出目录里生成 `QdBot.exe`。

## QQ 登录与 OneBot 设置

QdBot 不直接逆向登录 QQ。你需要使用 NapCat、Lagrange.OneBot 等 OneBot v11 适配器登录 QQ 账号。

适配器里需要配置：

- HTTP API 地址：默认可用 `http://127.0.0.1:3000`
- 事件上报地址：`http://127.0.0.1:8765`
- Access Token：如果适配器启用了 token，需要和 QdBot 界面里的 `OneBot Token` 保持一致

## AI API 设置

如果使用 OpenAI 官方接口：

- `AI Base URL`：`https://api.openai.com/v1`
- `AI API Key`：你的 API Key
- `AI 模型`：例如 `gpt-4o-mini`

如果使用其他 OpenAI 兼容平台，把 `AI Base URL` 改成该平台提供的 `/v1` 地址即可。

## 注意

- 请只在你有权限的 QQ 群和账号上使用。
- 触发前缀留空时，机器人会回复所有收到的消息，群里可能比较吵。
- 建议先把 `触发前缀` 设置为 `/ai` 或其他固定前缀。
