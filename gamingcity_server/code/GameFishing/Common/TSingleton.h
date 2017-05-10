////
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <memory>

template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if(_instance.get() == 0)
		{
			_instance = std::auto_ptr<T>(new T);
		}
		return _instance.get();
	};
	static void Destroy()
	{
		if(_instance.get() != 0)
		{
			_instance = std::auto_ptr<T>(0);
		}
	}
	static bool IsExist()
	{
		return _instance.get() != 0;
	}
protected:
	Singleton(){};
	~Singleton(){};

private://��ֹ��������͸�ֵ
	Singleton(const Singleton&){};
	Singleton& operator=(const Singleton&){};
private:
	static std::auto_ptr<T> _instance;
};

#define SingletonInstance(A)	template<> std::auto_ptr< A > Singleton< A >::_instance(0);

#define FriendBaseSingleton(A)	friend class std::auto_ptr< A >; friend class Singleton< A >;

//�÷�����
//class A : public Singleton<A>
//{
//protected:
//	A(){};
//	~A(){};
//	friend class Singleton<A>;
//	friend class std::auto_ptr<A>;
//};

//��cpp��������Ĵ��붨�徲̬�������������ǵ�������Ϊ A
//SingletonInstance(A);

#endif // __SINGLETON_H__
