#include <iostream>
#include <cstring>
#include <map>
#include <set>
#include <memory>
#include <vector>
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


// B类继承A类，定义了A类对象，如果A类的析构函数不是virtual，那么delete A对象实例的时候，就会导致B类内存泄漏。  我想不通，delete A类对象，为什么要是放b类对象，A类是基类，为什么要感知b类
// 这个问题提得非常精准，也恰恰是理解“虚析构函数”机制的最大难点。

// 要解开这个困惑，关键在于区分两个概念：“指针的类型”和“指针指向的对象的真实类型”。

// 你说的“delete A类对象”这个场景，其实在代码里通常体现为：

// cpp
// A* p = new B();  // 指针类型是 A，但指向的对象真实类型是 B
// delete p;        // 这里看起来是“delete A类对象”，但实际要删除的是 B 对象
// 你之所以想不通，是因为你把这行代码理解成了“我要删除一个 A 类型的对象”，但实际执行时，p 指向的是一块内存，这块内存里存放的是一个完整的 B 对象，它包含了 A 的部分和 B 自己的部分。

// 🧩 从内存布局的角度看
// 当 B 继承 A 时，一个 B 对象在内存中大致是：

// text
// +-------------------+
// | B 类自己的成员变量 |  ← B 的析构函数需要清理的部分
// +-------------------+
// | A 类的成员变量     |  ← A 的析构函数需要清理的部分
// +-------------------+
// 当你用 A* p = new B(); 时，p 指向的是整个 B 对象的起始地址。虽然你通过 A 类型的指针来操作它，但这块内存本身包含 B 的部分。

// 🔍 delete p 到底发生了什么？
// 执行 delete p; 时，编译器需要做两件事：

// 调用析构函数，清理对象占用的资源。

// 释放内存，把这块堆内存归还给操作系统。

// 关键问题出在第一步：编译器应该调用哪个析构函数？

// 如果 A 的析构函数不是虚函数，编译器只看 p 的静态类型（即 A*），直接调用 A::~A()。A 的析构函数并不知道自己是一个 B 对象的一部分，它只能清理 A 的成员，B 的成员没有被清理 → 内存泄漏。

// 如果 A 的析构函数是虚函数，编译器会通过对象的虚表（vtable）去查找真实类型，找到 B::~B()，然后调用它。B 的析构函数先清理自己的成员，然后再自动调用 A::~A() 清理 A 的部分 → 完整释放。

// 💡 所以，“泄漏”的不是内存本身，而是 B 那部分
// delete 会释放整块内存（包括 A 和 B 的部分），因为内存大小在 new 的时候就已经确定了。但如果析构函数不是虚的，B 的析构函数不会被调用，B 的成员中如果有动态分配的资源（比如另一个 new 出来的对象、文件句柄、网络连接等），这些资源就不会被释放。

// 内存本身会被释放，但 B 对象内部管理的资源（那些由 B 的构造函数申请、应由 B 的析构函数释放的资源）会泄露。






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




class MyClass {
public:
    ~MyClass() { cout << "析构" << endl; }
};

int main() {
    MyClass* arr = new MyClass[3];
    delete[] arr;   // 输出三次 "析构"，每个元素都被正确析构

    MyClass* obj = new MyClass();
    delete obj;     // 输出一次 "析构"

    // 错误示例（不要这样做）：
    // MyClass* arr2 = new MyClass[3];
    // delete arr2;  // 未定义行为！可能只析构第一个元素，或直接崩溃
}




//纯虚函数 及 多态应用

// 定义接口
class Drawable {
public:
    virtual void draw() const = 0;
    virtual ~Drawable() = default;  // 虚析构函数
};

// 实现接口
class Circle : public Drawable {
public:
    void draw() const override { cout << "画圆" << endl; }
};

class Rectangle : public Drawable {
public:
    void draw() const override { cout << "画矩形" << endl; }
};

int main() {
    // Drawable d;  // ❌ 错误！不能实例化抽象类

    vector<Drawable*> shapes;
    shapes.push_back(new Circle());    //此处体现父类指针指向子类对象
    shapes.push_back(new Rectangle());

    for (auto* s : shapes) {
        s->draw();   // ✅ 多态调用
    }
    return 0;
}


// 虚函数是怎么实现的
// 每一个含有虚函数的类都至少有有一个与之对应的虚函数表，其中存放着该类所有虚函数对应的函数指针（地址），

// 类的示例对象不包含虚函数表，只有虚指针；

// 派生类会生成一个兼容基类的虚函数表。




// 关键字static的作用
// 1）函数体内： static 修饰的局部变量作用范围为该函数体，不同于auto变量，其内存只被分配一次，因此其值在下次调用的时候维持了上次的值

// 2）模块内：static修饰全局变量或全局函数，可以被模块内的所有函数访问，但是不能被模块外的其他函数访问，使用范围限制在声明它的模块内

// 3）类中：修饰成员变量，表示该变量属于整个类所有，对类的所有对象只有一份拷贝

// 4）类中：修饰成员函数，表示该函数属于整个类所有，不接受this指针，只能访问类中的static成员变量

// 注意和const的区别！！！const强调值不能被修改，而static强调唯一的拷贝，对所有类的对象