/************************************************************************
*	AM_Cache.h -- Cache for performing background data loading			*
*				  Copyright (c) 2014 Justin Byers						*
* ===================================================================== *
* This file is part of AniMiku.											*
*																		*
* AniMiku is free software : you can redistribute it and / or modify	*
* it under the terms of the GNU General Public License as published by	*
* the Free Software Foundation, either version 3 of the License, or		*
* (at your option) any later version.									*
*																		*
* AniMiku is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of		*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the			*
* GNU General Public License for more details.							*
*																		*
* You should have received a copy of the GNU General Public License		*
* along with AniMiku. If not, see <http://www.gnu.org/licenses/>.		*
*************************************************************************/

#ifndef _CACHE_
#define _CACHE_
#include "AMTypes.h"

/*
	Threaded cache for background loading of data. Designed to load models and
	motion data, but is not really used currently.
*/
template <class T>
class AMCache
{
private:
	/*Handle to the worker thread*/
	HANDLE _thread;
	/*Object that actions will be performed on*/
	T *_obj;
	/*Is the data processing complete?*/
	bool _ready;

	/*Pointer to parameters needed to make call*/
	void *_pInitParams;
	/*Pointer to method thread will call*/
	void(*_pCacheFunc)(T*, void*);

	/*Method run by worker thread - performs all actions*/
	static DWORD WINAPI _threadFunc(LPVOID lpParameter)
	{
		AMCache<T> *pClass = (AMCache<T>*)lpParameter;
		pClass->_pCacheFunc(pClass->_obj, pClass->_pInitParams);
		pClass->_ready = true;

		return 0;
	}
public:
	AMCache(void* pInitParams, T* pUse, void (*pCacheFunc)(T*, void*))
	{
		_thread = NULL;
		_obj = pUse;
		_ready = false;
		_pInitParams = pInitParams;
		_pCacheFunc = pCacheFunc;
	}
	~AMCache()
	{
		
	}

	/*
		<T>* getResult()

		Get the data object that was loaded by the worker thread

		Parameters:			lock		block and wait until ready before returning
		Returns:			<T>*		pointer to object loaded by method, or null if not ready
										and lock=false
	*/
	T* getResult(bool lock)
	{
		if (lock && !_ready)
			while (!_ready) {}
		else if (!lock && !_ready)
			return NULL;
		
		return _obj;
	}

	/*
		bool isReady()

		Get if the data is ready to be retrieved

		Returns:			bool		Data ready to be retrieved
	*/
	bool isReady()
	{
		return _ready;
	}

	/*
		void run()

		Start the thread and perform processing.
	*/
	void run()
	{
		//disabled threading b/c causes problems with bullet physics
		//swap below 2 lines to use threading
		//_thread = CreateThread(NULL, 0, &_threadFunc, this, 0, 0);
		_threadFunc(this);
	}
};
#endif