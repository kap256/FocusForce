#pragma once

const TCHAR DELIMITER = _T('|');	//'|'はファイル名で使えないので、区切り文字にする

struct CFFMode {
public:
	bool mFixWinPos = true;
	bool mCancelInactive = false;

	bool IsAttach()
	{
		return mCancelInactive;
	}
	void FullPower()
	{
		mFixWinPos = true;
		mCancelInactive = true;
	}
	void FromString(TCHAR* param)
	{
		if (!param) {
			FullPower();
		} else {
			mFixWinPos = (param[0] != _T('0'));
			mCancelInactive = (param[1] != _T('0'));
		}
	}
	void ToString(TCHAR* param)
	{
		if (!param) {
			return;
		}
		param[0] = (mFixWinPos ? _T('1') : _T('0'));
		param[1] = (mCancelInactive ? _T('1') : _T('0'));
		param[2] = _T('\0');
	}
};