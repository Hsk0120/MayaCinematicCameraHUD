# MayaCinematicCameraHUD
Mayaでのシネマティック演出向けカメラHUD機能を提供するプラグイン。

## 使い方

1. Plug-in Manager、または `cmds.loadPlugin("MayaCinematicCameraHUD")` でプラグインをロードする。
2. `cmds.createNode("CameraHud")` でノードを作ると、Viewport 2.0 の各ビューに、そのビューのカメラの情報が重ねて表示される。

| 位置 | 表示内容(上から順) |
| --- | --- |
| 左上 | カメラ名 / 焦点距離 / カット番号(カメラのネームスペースの末尾3文字) / カメラの `camera_cache` アトリビュートの状態 |
| 右上 | 日付 / ユーザー名 |
| 左下 | 開始フレーム / 終了フレーム / 現在のフレーム |
| 右下 | タイムコード |
| 下中央 | シーンのファイル名 |

オーバースキャンが1以外のカメラでは、ゲートの内側に収まるように表示位置を寄せる。

## アトリビュート

| 名前(短い名前) | 型 | 既定値 | 範囲 | 内容 |
| --- | --- | --- | --- | --- |
| `textFontSize` (`tfs`) | float | 12 | 1〜128 | 文字サイズ(ピクセル) |
| `textBoxTransparency` (`tbt`) | float | 0.3 | 0〜1 | 文字背景の透明度。0で不透明、1で透明 |

## ビルド

CMake 3.15以降と、Mayaのdevkit(`DEVKIT_LOCATION` に指定)を使う。

```bat
set DEVKIT_LOCATION=C:/Program Files/Autodesk/Maya2026
cmake -S . -B build_2026 -DMAYA_VERSION=2026
cmake --build build_2026 --config Release
```

成果物は `release/plug-ins/windows/<Mayaのバージョン>/MayaCinematicCameraHUD.mll` に出力される。
Mayaのバージョンごとに対応するVisual Studioのツールセットが異なるため、必要に応じて `-T v142` のように指定する。
