# Catchrobo-Sample

回転機構(遊星歯車など)と直動機構(ラックアンドピニオンなど)で超音波センサーとロータリーエンコーダーを利用した簡単なフィードバック制御を行うためのサンプルスケッチ

## ファイル構成
```
catchrobo-sample/
├─ library/
│  ├─ AnalogMotorDriver.h       # モータードライバーの制御
│  └─ UltraSonic.h              # 超音波センサーの値取得
├─ linear_encoder/
│  └─ linear_encoder.ino        # ロータリーエンコーダーと直動機構
├─ linear_ultrasonic/
│  └─ linear_ultrasonic.ino     # 超音波センサーと直動機構
├─ rotary_encoder/
│  └─ rotary_encoder.ino        # ロータリーエンコーダーと回転機構
├─ LICENSE
└─ README.md
```

## 注意点
それぞれのスケッチを実行するためにはlibraryフォルダ内にあるヘッダファイルをそのスケッチのフォルダ内に移動させる必要があります。  
ESP32とDualShock4(MACアドレス書き込み済み)が必要です。  
DualShock4との通信用ライブラリはこの[リンク先](https://github.com/pablomarquez76/PS4_Controller_Host)にある他、ArduinoIDEのライブラリマネージャで`PS4Controller`と入力して検索することが出来ます。  

### あくまでもサンプルなので適宜改変して使用してください。

フィードバック制御自体は超脳筋P制御です。** ピン番号や各種の定数、特にPゲインは完全に適当に決めているため絶対にそのまま実行しないでください。 **どうなっても知りません。  

モータードライバー制御用ヘッダファイルは余計な機能がついていますが、`AnalogMotor_Base`クラスを継承した`AnalogMotor_ESP32`クラスが動作するので詳細な実装はその二つのクラス定義を参照してください。  

## 実装の詳細

ピン番号はnamespaceで管理している。モータードライバーのクラスが仕様により配列ポインタを渡さないといけないので、もし全部のピンを配列にまとめるなら先頭2つにモータードライバー用のピンが来るようにしないといけない。  
また、コントローラーとの接続用MACアドレスは00:00:00:00:00:00に設定してある。接続が安定しない場合はコントローラーの充電が不十分かも?(ESPとの相性も多分ある)  

<details>
<summary>

### Linear_Encoder</summary>

map_target関数内でコントローラーの〇/△/☐ボタンでそれぞれ10/20/30cmを目標位置length_targetに設定。離すと0cmに戻る。  
2相エンコーダーの片方のピンがON->OFF/OFF->ONする際に距離length_nowを増減させ、目標位置length_targetとの差分に0.1をとりあえずかけてモーターの回転速度ゲインとする。  
リミットスイッチの値はポーリングで取得して現在位置を調整する他、モーターがその方向に動かないように速度値を0にする。  
初期位置の設定は可動域の中央になっているので、機体を動かす際は初期位置から近い方にまず寄せて現在位置の構成をする必要がある。  
portENTER_CRITICALとportEXIT_CRITICALは割り込みで更新される値をloop関数から更新する際に衝突を回避するためのクリティカルセクション  

</details><details><summary>

### Linear_UltraSonic</summary>

map_target関数内でコントローラーの〇/△/☐ボタンでそれぞれ10/20/30cmを目標位置length_targetに設定。離すと0cmに戻る。  
現在位置は超音波センサーで取得する(ブロッキング処理)。Sensor初期化時にtimeoutで反射物がない時のタイムアウト時間を設定する。  
リミットスイッチの値はポーリングで取得して、モーターがその方向に動かないように速度値を0にする。  
反射物がセンサーに対して斜めになっていたり遠すぎたりすると値が正しく取得できない。  

</details><details><summary>

### Rotary_Encoder</summary>

map_target関数内で右ジョイスティックのXY値から算出した角度を目標角度angle_targetに設定。中心から半径10(ignore_range)以内にある場合はモーターを回転させない。  
2相エンコーダーの片方のピンがON->OFF/OFF->ONする際に角度angle_nowを増減させ、目標角度angle_targetとの差分にとりあえず5をかけてモーターの回転速度ゲインとする。  
原点用のリミットスイッチの値をポーリングで取得して現在角度を補正する。  
portENTER_CRITICALとportEXIT_CRITICALは割り込みで更新される値をloop関数から更新する際に衝突を回避するためのクリティカルセクション  

</details>