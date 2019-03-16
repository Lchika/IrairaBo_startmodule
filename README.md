# IrairaBo_startmodule

## 概要

- スタートモジュール用のArduinoコード

## ピン関係

|ピン番号|役割|
|-|-|
|3|スタートモジュールのコース接触判定|
|6|DIPスイッチbit0|
|7|DIPスイッチbit1|
|8|DIPスイッチbit2|
|9|DIPスイッチbit3|
|10|PC通信用ソフトウェアシリアルRX|
|11|PC通信用ソフトウェアシリアルTX|
|12|スタートモジュールのコース通過判定|
|A4(18)|D-sub2番ピン(SDA)|
|A5(19)|D-sub3番ピン(SCL)|
|GND|D-sub1番ピン|

- 上記ピン番号はすべて仮決め
  - というかコース接触判定・ゴール判定のピンを1個しか割り当ていないが、判定に複数ピンを使いたい場合もあるだろう…
  - そのときはコード修正必要
- I2C関係ピンは固定
- ソフトウェアシリアルのピン番号(RX, TX) → いくらでも変更可能。

## D-sub関係

- スレーブのアドレス指定：スタートモジュールに近い順に1,2,3...  
- D-sub関係はI2C(2,3番ピン)のみに集約し、D-subの4,5番ピン(ゴール通知・コース接触通知)は不要となった。
- ただし、上記対応により各モジュールの通過判定が必須になった
  - どのモジュールと通信すればよいかわからないとI2C通信できないため

## 処理の流れ

1. スタート待機状態  
- PCにスタートメッセージ('s')を投げる。
- そのままPCから's'が返ってきたら 2. スタートモジュール通過中状態 に遷移する。
- …ように今は実装してあるが、スタート検知できるようにするなら、以下のような変更が必要。
  - スタート検知したら 2. スタートモジュール通過中状態 に遷移するようにする。
2. スタートモジュール通過中状態 
- コース接触を検知した場合
  - PCへ当たった判定メッセージ('h')を投げる。
  - そのままPCから'h'が返ってくるまで待機。
- モジュール通過を検知した場合
  - PCへ通過/ゴール判定メッセージ('t')を投げる。
  - そのままPCから't'が返ってきたらスレーブ1との通信を開始し、 3. スレーブモジュール通過中状態 に遷移する。
3. スレーブモジュール通過中状態
- 一定時間ごとにスレーブモジュールの状態を確認する。
- スレーブモジュールの状態に応じて必要であればPCと通信を行う。
- 通信対象モジュールの切り替えもDsub関係管理クラスの`handle_dsub_event()`の中で行われる。