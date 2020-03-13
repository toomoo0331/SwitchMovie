# Switch Movie

## Dependencies
- Boost(1.58.0)
- OpenCV(2.4.13)
- TBB

## Runnig the Project
- In config.ini, change line 3 with the absolute file path to the Switch Movie project folder.
```ini:config.ini
root = ABSOLUTE PATH TO PROJECT FOLDER
```


## ダイクストラ法のやつ
- In test/switch_movie_csv.cpp, head.csvのとこのパスとrootのとこのパスとdirectoryが指す動画が入ってるディレクトリのパスを変える！！
```switch_movie_csv.cpp
root = ABSOLUTE PATH TO PROJECT FOLDER
directory = 動画が５つ入ってるディレクトリ
ifs = ５つのスコアが総フレーム分入ってるcsvファイル
str = 動画までのパス
```