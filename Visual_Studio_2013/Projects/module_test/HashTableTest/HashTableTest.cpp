// HashTableTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include  <string>
using  std::string;

#include  <xstring>
using  std::wstring;

#include <hash_map>
using namespace std;

#include <google/sparse_hash_map>
using google::sparse_hash_map;      // namespace where class lives by default
//#include <google/dense_hash_map>
//using google::dense_hash_map;      // namespace where class lives by default

#include  <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	//sparse_hash_map<const wchar_t *, int> m_TestCache;
	//sparse_hash_map<wchar_t [MAX_PATH], int> m_TestCache;
	//sparse_hash_map<string, string, Hasher, Hasher> m_TestCache;//,str_hash<wchar_t *>, eqstr> ;;//
	sparse_hash_map<wstring, int> m_TestCache;

	//sparse_hash_map::key_type
/*
	m_TestCache[_T("a")] = 1;
	m_TestCache[_T("b")] = 2;
	m_TestCache[_T("c")] = 3;
	m_TestCache[_T("dd")] = 4;
	m_TestCache[_T("e")] = 5;
	m_TestCache[_T("f")] = 6;
	m_TestCache[_T("g")] = 7;
	m_TestCache[_T("h")] = 8;
	m_TestCache[_T("i")] = 9;
	m_TestCache[_T("j")] = 10;
*/

  m_TestCache[_T("dd")] = 4;

  //sparse_hash_map<string, string, Hasher, Hasher>::iterator iter;
  sparse_hash_map<wstring, int>::iterator iter;
  for (iter = m_TestCache.begin(); iter != m_TestCache.end(); ++iter)
	_tprintf(_T("%s = %d\n"), iter->first.c_str(), iter->second);

  iter = m_TestCache.find(_T("dd"));
  if(iter != m_TestCache.end())
  	_tprintf(_T("%s = %d\n"), iter->first.c_str(), iter->second);

wstring str3(_T("dd"));
  m_TestCache.erase(str3);
	//wchar_t *pFilePath = (wchar_t *)malloc(260+1);
	//if(!pFilePath)
	//	return 0;
	wchar_t FilePath[260+1];
	//wchar_t *pFilePath = (wchar_t *)malloc(260+1);
	//_tcscpy_s(pFilePath, 260+1, _T("xxxxx"));
	//wstring str1, str2;
	//str1 = _T("xxxxx");
	//str2 = _T("xxxxx");
	wstring str1(_T("xxxxx"));
	wstring str2(_T("xxxxx"));
	_tcscpy_s(FilePath, 260+1, _T("xxxxx"));
  	m_TestCache[str1] = 110;

  iter = m_TestCache.find(FilePath);
  if(iter != m_TestCache.end())
  	printf("\n%s = %d\n", iter->first.c_str(), iter->second);
/*
	wchar_t *pFilePath = (wchar_t *)malloc(260+1);
	_tcscpy_s(pFilePath, 260+1, _T("xxxxx"));
	m_TestCache[pFilePath] = 110;

  iter = m_TestCache.find(_T("xxxxx"));
  if(iter != m_TestCache.end())
  	_tprintf(_T("%s = %d\n"), iter->first, iter->second);
*/
	return 0;
}



