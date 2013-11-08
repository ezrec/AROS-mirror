
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_ARRAY_
#define _RAR_ARRAY_

template <class T>
class Rar_Array {
public:
	Rar_Array();
	Rar_Array(int Size);
	~Rar_Array();
	inline void CleanData();
	inline T& operator [](int Item);
	inline int Size();
	void Add(int Items);
	void Alloc(int Items);
	void Reset();
	void operator = (Rar_Array<T> &Src);
	void Push(T Item);

private:
	T *Buffer;
	int BufSize;
	int AllocSize;
};

template <class T> void Rar_Array<T>::CleanData()
{
	Buffer=NULL;
	BufSize=0;
	AllocSize=0;
}


template <class T> Rar_Array<T>::Rar_Array()
{
	CleanData();
}


template <class T> Rar_Array<T>::Rar_Array(int Size)
{
	Buffer=(T *)malloc(sizeof(T)*Size);
	if (Buffer==NULL && Size!=0)
		rar_out_of_memory();

	AllocSize=BufSize=Size;
}


template <class T> Rar_Array<T>::~Rar_Array()
{
	if (Buffer!=NULL)
		free(Buffer);
}


template <class T> inline T& Rar_Array<T>::operator [](int Item)
{
	return(Buffer[Item]);
}


template <class T> inline int Rar_Array<T>::Size()
{
	return(BufSize);
}


template <class T> void Rar_Array<T>::Add(int Items)
{
	BufSize+=Items;
	if (BufSize>AllocSize)
	{
		int Suggested=AllocSize+AllocSize/4+32;
		
		int NewSize = BufSize;
		if ( NewSize < Suggested )
			NewSize = Suggested;

		Buffer=(T *)realloc(Buffer,NewSize*sizeof(T));
		if (Buffer==NULL)
			rar_out_of_memory();
		AllocSize=NewSize;
	}
}


template <class T> void Rar_Array<T>::Alloc(int Items)
{
	if (Items>AllocSize)
		Add(Items-BufSize);
	else
		BufSize=Items;
}


template <class T> void Rar_Array<T>::Reset()
{
	if (Buffer!=NULL)
	{
		free(Buffer);
		Buffer=NULL;
	}
	BufSize=0;
	AllocSize=0;
}


template <class T> void Rar_Array<T>::operator =(Rar_Array<T> &Src)
{
	Reset();
	Alloc(Src.BufSize);
	if (Src.BufSize!=0)
		memcpy((void *)Buffer,(void *)Src.Buffer,Src.BufSize*sizeof(T));
}


template <class T> void Rar_Array<T>::Push(T Item)
{
	Add(1);
	(*this)[Size()-1]=Item;
}

#endif

