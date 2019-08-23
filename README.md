# FocusForce
「画面が切り替わったから、ゲームを停止するよ。」……。  
ふ、ふざけるな、俺はゲーム演出を見ながらツイートしたいんだ！　勝手にゲームを停止するようなマネは許さん！

## 使う
- 起動するとタスクトレイに常駐
	- アイコンを左クリックで即終了
	- 右クリックするとダイアログがでる。気が向いたら設定変更とかできるようにしたい。

## 動作
### WM_ACTIVATEメッセージによるアクティブウインドウ切替通知を阻止
- WM_ACTIVATE - WA_INACTIVE を受け取る事で、画面が切り替わったと認識するゲーム対策
- SetWindowsHookExで他プロセスへのメッセージを取得できる
	- しかし、WH_GETMESSAGE や WH_CALLWNDPROCRET では WM_ACTIVATE が取得できない様子。
	- WH_CALLWNDPROC でなら WM_ACTIVATE が取得できるが、これでフックした場合、メッセージの変更ができない。
	- なので、WM_ACTIVATE - WA_INACTIVE が通知された場合、すこし時間をおいて WM_ACTIVATE - WA_ACTIVE を再送して、効果を打ち消す。

### GetForegroundWindow関数の偽装
- GetForegroundWindow() の結果が自分のウインドウハンドルでなければ、画面が切り替わったと認識するゲーム対策
- GetForegroundWindow() を上書きして、（多分）自分のウインドウハンドル（だろう）を返すようにさせる。
