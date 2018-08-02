/*

Singleton Class

*/


#ifndef SINGLETON_H
#define SINGLETON_H

#pragma once

namespace NYX {

template<typename ACTUAL_CLASS>
class NYX_EXPORT SingletonClass
{
public:

    SingletonClass();

    virtual ~SingletonClass();

    static ACTUAL_CLASS* GetInstance();

private:

    static SingletonClass* uniqueInstance;

};

//------------------------------
// Implementation
//------------------------------

template<typename ACTUAL_CLASS>
SingletonClass<ACTUAL_CLASS>* SingletonClass<ACTUAL_CLASS>::uniqueInstance = nullptr;

template<typename ACTUAL_CLASS>
SingletonClass<ACTUAL_CLASS>::SingletonClass()
{
	assert(!uniqueInstance);
	uniqueInstance = this;
}

template<typename ACTUAL_CLASS>
SingletonClass<ACTUAL_CLASS>::~SingletonClass()
{
	assert(uniqueInstance);
	uniqueInstance = nullptr;
}

template<typename ACTUAL_CLASS>
ACTUAL_CLASS* SingletonClass<ACTUAL_CLASS>::GetInstance()
{
	return static_cast<ACTUAL_CLASS*>(uniqueInstance);
}

}

#endif // SINGLETON_H
