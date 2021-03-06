/*
** Version  1.0.0.1
** Date     2018.12.01
** Author   zax
** Copyright (C) Since 2009 Zaxai.Com
*/
#pragma once
class ZOperatorRegedit
{
public:
	static BOOL SetAutoRun(const CString & strExePath, LPCTSTR lpValueName, BOOL nIsAutoRun);
	static BOOL CheckAutoRun(const CString & strExePath, LPCTSTR lpValueName);
	static BOOL SetREG_SZData(LPCTSTR lpSubKey, LPCTSTR lpValueName, const CString & strData);
	static BOOL GetREG_SZData(LPCTSTR lpSubKey, LPCTSTR lpValueName, CString & strData);
	static BOOL SetREG_DWORDData(LPCTSTR lpSubKey, LPCTSTR lpValueName, const DWORD & dwData);
	static BOOL GetREG_DWORDData(LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD & dwData);
};

