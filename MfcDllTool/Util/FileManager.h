#pragma once
#include <vector>
#include <cstring>
#include <map>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;
#define MAX_STR_LEN  1000  // �������ļ�������ַ�����󳤶�

struct	MyFileNameStruct
{
	CString mName;
	CTime mTime;

	MyFileNameStruct(CString name,CTime time)
	{
		mName=name;
		mTime=time;
	}

	CString GetTimeString()
	{
		CString date = mTime.Format("%Y-%m-%d %H:%M:%S");
		return date;
	}
};

namespace FileManager
{
	// �ļ���
	DLL_API BOOL CheckFolderExist(CString FolderPath);
	DLL_API BOOL CheckFileExist(CString FilePath);

	DLL_API BOOL CreateMuliteDirectory(CString FolderPath);
	DLL_API void DeleteDirectory(CString FolderPath);

	DLL_API std::vector<MyFileNameStruct> GetAllFileList(CString   strParent);
	// �ļ�·��
	DLL_API CString GetAppPath();  // exeȫ·��
	DLL_API CString GetFilePath(CString strPathName);  // ��ȡ·��
	DLL_API CString GetFileName(CString strPathName);  // �ļ���+��׺��
	DLL_API CString GetExtName(CString strPathName);   // ��׺��
	DLL_API CString GetFileTitle(CString strFileName); // �ļ���


	///INI ��д����
	DLL_API int	    ReadIniToInt    (CString strFilePath, CString strSection, CString strSectionKey);
	DLL_API double  ReadIniToDouble (CString strFilePath, CString strSection, CString strSectionKey);
	DLL_API CString ReadIniToString (CString strFilePath, CString strSection, CString strSectionKey);	
	DLL_API void	WriteIntToIni	(CString strFilePath, CString strSection, CString strSectionKey, int cfg);
	DLL_API void	WriteDoubleToIni(CString strFilePath, CString strSection, CString strSectionKey, double cfg);
	DLL_API void	WriteStringToIni(CString strFilePath, CString strSection, CString strSectionKey, CString cfg);

	DLL_API vector<CString> ReadSectionNames(CString mPath);
	DLL_API vector<CString> ReadChildsOnGroup(CString mPath, CString mGroupName);
	DLL_API BOOL IsSectionExits(CString mPath, CString section);
	DLL_API BOOL DeleteSection(CString mPath, CString section);

	//�ı����� TXT  CSV
	DLL_API BOOL SaveFile(CString str, CString strFilePath, BOOL app=TRUE, BOOL hide=FALSE); // �����ַ���
	DLL_API BOOL SaveFile(CString lineString, CString path, CString bkPath, BOOL app=TRUE);
	DLL_API BOOL SaveFile(vector<CString> strVec, CString strFilePath, CString SpitChar=_T(","), BOOL app=TRUE); // ����һ������
	DLL_API BOOL SaveFile(vector<vector<CString>> strVecVec, CString strFilePath, CString SpitChar=_T(","), CString strSencodSign=_T("\n"), BOOL app=TRUE); // ������������
	DLL_API BOOL ReadFileByLine(vector<CString> &strVec, CString strFilePath); // ���ж�ȡ

	DLL_API long FileTotalLines(CString strFilePath, bool removeEmptyLine=true);


	//ϵͳ����
	static std::map<CString, CString> mConfigMap;
	static CString mIniPath;
	DLL_API void InitConfigMap(CString mPath);
	DLL_API CString GetConfig(CString mName);
	DLL_API void SetConfig(CString mName,CString mValue,bool flagAutoSave=true);
	DLL_API void SetConfig(CString mName,int mValue,bool flagAutoSave=true);
	DLL_API void SetConfig(CString mName,float mValue,bool flagAutoSave=true);
	DLL_API void SetConfig(CString mName,double mValue,bool flagAutoSave=true);
};

