#include <iostream>
#include <cstring>
#include <map>
#include <set>
#include <memory>
using namespace std;

class A {
public:
    int a;
public:
    A(int val) : a(val) {}
};
    // A(int val) : a(val) {}
    // A(int val) {
    //     a = val;
    // }
        
    
class B : virtual public A {
    int b;
public:
    B(int val_a, int val_b) : A(val_a), b(val_b) {}
};
    //基类构造必须在“初始化列表”中进行
    // B(int val_a, int val_b) : A(val_a)
    // {
    //     b = val_b;
    // }


class C : virtual public A {
    int c;
public:
    C(int val_a, int val_c) : A(val_a), c(val_c) {}
};

class D : public B, public C {
    int d;
public:
    D(int val_a, int val_b, int val_c, int val_d) : A(val_a), B(val_a, val_b), C(val_a, val_c), d(val_d) {}
    //D(int val_a, int val_b, int val_c, int val_d) : B(val_a, val_b), C(val_a, val_c), d(val_d) {}
    // 但 A 是虚继承
    // 因为 B 和 C 都是 virtual public A，所以 A 是“共享基类”。

    // 这就意味着：

    // A 只会被构造一次
    // 这个一次构造需要由最远派生类 D 来负责
    // 也就是说，D 构造函数里要显式传入 A 的初始化参数，才能保证 A 被正确构造。

    // 3. 你现在这行缺了 A(val_a) 的显式初始化
};




int main1() {
    D d(1, 2, 3, 4);
    cout << "d: " << d.a << endl;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

// 成员变量
// struct 里默认是 public
// class 里默认是 private



class animal {
public:
    virtual void speak()
    {
        cout << "animal speak" << endl;
    }
};

class dog : public animal {
public:
    void speak() override
    {
        cout << "dog speak" << endl;
    }
};

class cat : public animal {
public:
    void speak() override
    {
        cout << "cat speak" << endl;
    }

};

void test_animal(animal& a)
{
    a.speak();
}

int main2()
{
    cat c;
    dog d;
    test_animal(c);
    test_animal(d);
    return 0;
}


class base {
public:
    virtual ~base() {
        cout << "base destructor" << endl;
    }

};

class derived : public base {
public:
    ~derived() {
        cout << "derived destructor" << endl;
    }
};

int main4()
{
    base* b = new derived;
    derived* d = new derived;
    cout << "delete b" << endl;
    delete b; 
    cout << "delete d" << endl;
    delete d; 
    return 0;
}

// 因为每个对象内部都有一个隐藏的指针（vptr），指向一张虚函数表（vtable）。当基类析构函数是虚函数时，delete 操作会通过这个指针，在运行时找到当前对象真正的类型，从而调用正确的析构函数。


template <typename T>
class MyClass {
public:
    int tmp;
    void fun(T val) {
        tmp = val;
        cout << "tmp: " << tmp << endl;
    }
};

template <typename T>
class MyClass2 {
public:
    char a[100];
    void fun(T val) {
        strcpy(a, val);
        printf("tmp: %s\n", a);
    }
};

int main6()
{
    MyClass<int> obj1;
    obj1.fun(10);

    MyClass2<const char*> obj2;
    obj2.fun("Hello");

    return 0;
}

int main7() {
    map<int, string> myMap;
    myMap.insert(pair<int, string>(1, "one"));
    myMap[1] = "one_1";
    myMap[2] = "two";

    for (map<int, string>::iterator it = myMap.begin(); it != myMap.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    return 0;
}



//仿函数
struct MyFunctor {
    void operator()(int x) {
        cout << "MyFunctor called with value: " << x << endl;
    }
};

class functor {
    public:
    int operator()(int x, int y) {
        return x + y;
    }

};

int main8() {
    
    functor f;
    int result = f(3, 4); // 调用仿函数
    cout << "Result: " << result << endl;

    MyFunctor functor;
    functor(42); // 调用仿函数

    return 0;
}


//仿函数（Functor）类型
struct MyFunctor2 {
    bool operator()(const int &x, const int &y) const{
        return (x > y); // 重载operator()，实现降序排列
    }
};

int main9() {
    set<int, MyFunctor2> mySet;
    mySet.insert(3);
    mySet.insert(1);
    mySet.insert(4);

    for (const auto &elem : mySet) {
        cout << elem << " ";
    }
}


//unique_ptr 是轻量级的智能指针，它独占所管理的对象，不能被拷贝，只能被移动。
class MyClass {
public:
    ~MyClass() { cout << "析构" << endl; }
};

int main10() {
    unique_ptr<MyClass> p1 = make_unique<MyClass>();  // C++14 起推荐
    // unique_ptr<MyClass> p2 = p1;  // ❌ 编译错误！不能拷贝
    unique_ptr<MyClass> p2 = std::move(p1);  // ✅ 可以转移所有权

    // p1 现在为空，p2 拥有对象
    if (!p1) cout << "p1 为空" << endl;
    // 离开作用域时，p2 自动析构，释放内存
    return 0;
}

//shared_ptr 通过引用计数来管理资源：每多一个 shared_ptr 指向同一对象，计数器 +1；每销毁一个，计数器 -1。当计数器归零时，自动释放资源。

int main11() {
    shared_ptr<int> p1 = make_shared<int>(42);     //shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2 = p1;  // ✅ 可以拷贝，引用计数变为 2

    cout << *p1 << endl;      // 42
    cout << p1.use_count() << endl;  // 2

    p2.reset();               // 释放 p2，引用计数变为 1
    // 离开作用域时，p1 销毁，引用计数归零，释放内存
    return 0;
}


//weak_ptr 不参与引用计数，它像一个“观察者”，可以访问 shared_ptr 管理的资源，但不会阻止资源释放。

struct Node {
    shared_ptr<Node> next;
    // weak_ptr<Node> next;   // 改为 weak_ptr 即可打破循环
    ~Node() { cout << "Node 析构" << endl; }
};

int main12() {
    auto n1 = make_shared<Node>();
    auto n2 = make_shared<Node>();
    n1->next = n2;
    n2->next = n1;   // 循环引用！两个对象永远不会被释放

    // 如果把其中一个改成 weak_ptr，就能正常析构
    return 0;
}