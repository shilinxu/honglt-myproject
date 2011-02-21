//
// Randomize.h
//

#ifndef RANDOMIZE_H__INCLUDED
#define RANDOMIZE_H__INCLUDED

#pragma once

class COM_SERVICE_CLASS CRandomize  
{
// Construction
// protected:
// 	CRandomize();
// 	virtual ~CRandomize();

// Operations
public:
	static	void	CreateRandom();
public:
	static	WORD	GetRandom(WORD *pnId = NULL) /*const*/;
	static	DWORD	GetRandom(DWORD *pnId = NULL) /*const*/;
};

#endif // RANDOMIZE_H__INCLUDED
