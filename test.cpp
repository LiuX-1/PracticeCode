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
class MyClassUnique {
public:
    ~MyClassUnique() { cout << "析构" << endl; }
};

int main10() {
    unique_ptr<MyClassUnique> p1 = make_unique<MyClassUnique>();  // C++14 起推荐
    // unique_ptr<MyClassUnique> p2 = p1;  // ❌ 编译错误！不能拷贝
    unique_ptr<MyClassUnique> p2 = std::move(p1);  // ✅ 可以转移所有权

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




class MyClassArray {
public:
    ~MyClassArray() { cout << "析构" << endl; }
};

int main14() {
    MyClassArray* arr = new MyClassArray[3];
    delete[] arr;   // 输出三次 "析构"，每个元素都被正确析构

    MyClassArray* obj = new MyClassArray();
    delete obj;     // 输出一次 "析构"

    // 错误示例（不要这样做）：
    // MyClassArray* arr2 = new MyClassArray[3];
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

int main13() {
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



// 什么情况下会调用拷贝构造函数（三种情况）
// 系统自动生成的构造函数：普通构造函数和拷贝构造函数 （在没有定义对应的构造函数的时候）

// 生成一个实例化的对象会调用一次普通构造函数，而用一个对象去实例化一个新的对象所调用的就是拷贝构造函数

// 调用拷贝构造函数的情形：

// 1）用类的一个对象去初始化另一个对象的时候

// 2）当函数的参数是类的对象时，就是值传递的时候，如果是引用传递则不会调用

// 3）当函数的返回值是类的对象或者引用的时候





// 全局变量在整个文件（甚至整个程序）中可见

// global.cpp

// cpp
// // 定义全局变量
// int g_counter = 0;
// other.cpp

// cpp
// // 声明全局变量（不分配内存，只告诉编译器“这个变量存在”）
// extern int g_counter;

// void func() {
//     g_counter++;   // ✅ 可以访问，修改的是 global.cpp 中定义的那个变量
// }



// 对比：静态全局变量
// 如果给全局变量加上 static 修饰，它的作用域就会被限制在当前文件内：

// cpp
// // global.cpp
// static int s_hidden = 42;   // 只在 global.cpp 中可见


int main15(){
    vector<int> vec(10);  //有这种构造函数
    cout << vec.size() << endl;
    cout << vec[0] << endl;
    return 0;
}



//友元函数和友元类

class MyClassFriend {
private:
    int tmp;
public:
    MyClassFriend(int val) : tmp(val) {}
    friend int printTmp(const MyClassFriend& obj);  // 声明友元函数    
};

int printTmp(const MyClassFriend& obj) {
    cout << "tmp: " << obj.tmp << endl;  // 可以访问私有成员
    return 0;
}

int main123() {
    MyClassFriend obj(42);
    printTmp(obj);
    return 0;
}


//友元类
class A0805 {
private:
    int data = 10;
    friend class B0805;   // 声明 B 是 A 的友元类
};

class B0805 {
public:
    void showA(const A0805& a) {
        cout << a.data << endl;  // ✅ B 的所有函数都能访问 A 的私有成员
    }
};




// 虚函数、纯虚函数怎么实现
// 用virtual关键字申明的函数叫做虚函数，虚函数肯定是类的成员函数； 
// 存在虚函数的类都有一个一维的虚函数表叫做虚表，类的对象有一个指向虚表开始的虚指针。
// 虚表是和类对应的，虚表指针是和对象对应的； 
// 多态性是一个接口多种实现，是面向对象的核心，分为类的多态性和函数的多态性；
// 多态用虚函数来实现，结合动态绑定； 纯虚函数是虚函数再加上 = 0； 
// 抽象类是指包括至少一个纯虚函数的类。纯虚函数:virtual void fun()=0;
// 即抽象类！抽象基类不能定义对象。必须在子类实现这个函数，即先有名称，没有内容，
// 在派生类实现内容。



// 构造函数能不能是虚函数
// 不能。构造一个对象时，必须知道对象实际类型，而虚函数是在运行期间确定实际类型的。
// “实际类型”指的是“对象本身的类型”，而不是“虚函数的类型”。
// 而在构造一个对象时，由于对象还未构造成功，编译器就无法知道对象的实际类型，
// 是该类本身，还是派生类，还是其他。 
// 虚函数的执行依赖于虚函数表，而虚函数表是在构造函数中进行初始化的，
// 即初始化虚表指针（vptr），使得正确指向虚函数表。
// 而在构造对象期间，虚函数表（vtable）还没有被初始化，将无法进行。



// C++11新特性了解吗
// 1、新增容器std::array 保存在栈内存中，相比堆内存中的 std::vector，
// 我们能够灵活的访问这里面的元素，从而获得更高的性能。 
// 2、auto 和 decltype 这两个关键字实现了类型推导 3、替代NULL的nullptr 
// 4、三种智能指针帮助内存管理(说一下名称)：unique_ptr、shared_ptr 和 weak_ptr。 
// 5、C++11 引入了基于范围的迭代写法，比如基于范围的for循环，用一个冒号就可实现遍历，
// 我们拥有了能够写出像 Python一样简洁的循环语句。




// 堆和栈的区别，以及为什么栈效率高
// 堆是由低地址向高地址扩展；栈是由高地址向低地址扩展。 
// 堆中的内存需要手动申请和手动释放；
// 栈中内存是由OS自动申请和自动释放，存放着参数、局部变量等内存。
// 堆中频繁调用malloc和free,会产生内存碎片，降低程序效率；
// 而栈由于其先进后出的特性，不会产生内存碎片。 
// 堆的分配效率较低，而栈的分配效率较高。

// 栈的效率高的原因： 栈是操作系统提供的数据结构，
// 计算机底层对栈提供了一系列支持：分配专门的寄存器存储栈的地址，
// 压栈和入栈有专门的指令执行；
// 而堆是由C/C++函数库提供的，机制复杂，需要一些列分配内存、合并内存和释放内存的算法，
// 因此效率较低。

// unique_ptr（替换auto_ptr）unique_ptr实现独占式拥有或严格拥有概念，
// 保证同一时间内只有一个智能指针可以指向该对象。
// 它对于避免资源泄露(例如“以new创建对象后因为发生异常而忘记调用delete”)特别有用。
// 采用所有权模式。

// unique_ptr<string> p3 (new string ("auto"));
// unique_ptr<string> p4；
// p4 = p3;//此时会报错！！
//编译器认为p4=p3非法，避免了p3不再指向有效数据的问题。因此，unique_ptr比auto_ptr更安全。
//编译器认为p4=p3非法，避免了p3不再指向有效数据的问题。因此，unique_ptr比auto_ptr更安全



//内存对齐
// CPU在读取内存时，并不是按字节逐个读取，而是按“字”（Word）的大小（如4字节或8字节）来批量读取。
// 为了提升读取效率，编译器会要求数据在内存中的起始地址，必须是它自身大小的整数倍。这就是内存对齐。
// 为什么要对齐？
// 硬件层面：CPU访问未对齐的内存时，可能需要两次总线操作，效率降低；有些架构（如ARM）甚至直接报错。
// 编译器层面：编译器会在结构体中插入“填充字节”（padding），让每个成员都落在对齐的边界上。

// struct A {
//     char a;   // 1 字节
//     int b;    // 4 字节
//     char c;   // 1 字节
// };
// 直觉上，sizeof(A) 可能是 1+4+1=6，但实际通常是 12 字节（在 4 字节对齐的系统上）。

// 什么是位域？

// 位域允许你精确指定一个成员占用多少比特位，通常用于节省内存或直接操作硬件寄存器。

// cpp
// struct Status {
//     unsigned int flag1 : 1;   // 占1位
//     unsigned int flag2 : 1;   // 占1位
//     unsigned int value  : 6;   // 占6位
// };
// 这里 Status 只占 1 个字节（8位），而不是 3 个 int（12 字节）。这在嵌入式开发或网络协议解析中非常实用。



// 什么是组合？
// 1)一个类里面的数据成员是另一个类的对象，即内嵌其他类的对象作为自己的成员；
// 创建组合类的对象：首先创建各个内嵌对象，难点在于构造函数的设计。
// 创建对象时既要对基本类型的成员进行初始化，又要对内嵌对象进行初始化。 
// 2)创建组合类对象，构造函数的执行顺序：先调用内嵌对象的构造函数，
// 然后按照内嵌对象成员在组合类中的定义顺序，与组合类构造函数的初始化列表顺序无关。
// 然后执行组合类构造函数的函数体，析构函数调用顺序相反。

class Engine {
public:
    int tmp;
    Engine() { cout << "Engine 构造" << endl; }
    Engine(int val) {
        tmp = val;
        cout << "Engine 带参构造" << endl;
    }
    // Engine(int val) : tmp(val) {
    //     cout << "Engine 带参构造" << endl;
    // }
    ~Engine() { cout << "Engine 析构" << endl; }
};

class Wheel {
public:
    int tmp;
    Wheel() { cout << "Wheel 构造" << endl; }
    Wheel(int val) {
        tmp = val;
        cout << "Wheel 带参构造" << endl;
    }
    ~Wheel() { cout << "Wheel 析构" << endl; }
};

class Car {
private:
    Engine engine;   // Car 拥有一个 Engine 对象（组合）
    Wheel wheel;     // Car 拥有一个 Wheel 对象（组合）
public:
    Car() { cout << "Car 构造" << endl; }
    ~Car() { cout << "Car 析构" << endl; }
};

// 如果组合类的构造函数需要通过初始化列表向成员对象传递参数，写法如下：
class Car2 {
private:
    Engine engine;
    Wheel wheel;
public:
    // engine(2000) 表示调用 Engine 的带参构造函数，传递参数 2000
    Car2() : engine(2000), wheel(18) {
        cout << "Car2 构造" << endl;
    }
};
// 这时候，engine(2000) 和 wheel(18) 在初始化列表中的顺序，
// 仍然不会改变 engine 和 wheel 的实际构造顺序（由声明顺序决定）。


// 初始化列表
// 初始化列表有两种完全不同的用法，一种在类里面（构造函数初始化列表），另一种在类外面（变量定义时）。

// 两种“初始化列表”的本质区别
// 对比维度	构造函数初始化列表	统一初始化（{} 初始化变量）
// 使用位置	只在类的构造函数定义中	在任何变量定义的地方（函数内、全局、类成员定义等）
// 语法形式	ClassName(参数) : 成员1(值), 成员2(值) { }	int a{10}; vector<int> v{1,2,3};
// 核心目的	初始化类的成员变量（尤其是 const、引用、无默认构造的成员）	初始化任意变量（基础类型、数组、结构体、容器）
// 能否在类外使用	❌ 只能在类定义内部使用	✅ 可以在任何地方使用


class MyClass0810 {
private:
    const int id;       // const 成员
    int& ref;           // 引用成员
    std::string name;
public:
    // ✅ 这里用的是"构造函数初始化列表"，只能在类里面
    MyClass0810(int i, int& r, const std::string& n)
        : id(i), ref(r), name(n) {   // ← 这是构造函数初始化列表
    }
};


struct Point {
    int x;
    int y;
};

int main0810() {
    // ✅ 定义变量时使用 {} 初始化，可以在任何地方
    int a{10};                     // 基础类型
    int arr[3]{1, 2, 3};           // 数组
    std::vector<int> v{1, 2, 3};   // 容器
    Point p{10, 20};               // 结构体

    int b = 5;
    MyClass0810 obj{5, b, "hello"};  // 也可以用于类对象
}


//  一个常见的混淆点
// 如果看到类定义中成员变量直接使用 {} 初始化，这既不是构造函数初始化列表，也不是普通的变量初始化，而是 C++11 引入的类内成员初始化器（In-class Member Initializer）：

class MyClass08101 {
private:
    int a{10};        // ✅ 类内成员初始化器（不是构造函数初始化列表）
    int b = 20;       // ✅ 也支持 = 语法
public:
    MyClass08101() {}      // a 和 b 都会被初始化为 10 和 20
};

// 这种用花括号 {} 初始化变量的语法，确实是在 C++11 标准中才被正式引入，并作为一项核心新特性推广开来的



//确认编译环境是否支持c++11

int main0811() {
    cout << "c++ version" << __cplusplus << endl;
    return 0;
}



//a 和&a 有什么区别
// int a[5] = {1, 2, 3, 4, 5};

// cout << a << endl;      // 输出一个地址，比如 0x1000
// cout << &a << endl;     // 输出同样的地址，也是 0x1000
// cout << a + 1 << endl;  // 输出 0x1004（跳过 1 个 int，+4 字节）
// cout << &a + 1 << endl; // 输出 0x1014（跳过整个数组，+20 字节）
// 从这个输出可以清楚地看到：a 和 &a 虽然指向同一个起点，但 a+1 和 &a+1 的偏移量完全不同。 这就是类型不同导致的直接结果。


// int a[5];
// cout << sizeof(a);    // 输出 20（= 5 * sizeof(int)）
// cout << sizeof(&a);   // 输出 8（在64位系统上，指针大小为8字节）

// 在 sizeof(a) 中，a 保持为数组类型，所以返回整个数组的大小。而在 sizeof(&a) 中，
// &a 已经被显式取地址，返回的是指针的大小。
// 这进一步证明了：数组名 a 本身是数组类型，&a 则是一个指向数组的指针。

// a 数组首地址，a+1 数组的下一个元素，sizeof(a) 这个数组大小
// &a 数组首地址，&a+1 跳过整个数组， sizeof（&a） 是指针大小


//  一句话总结
// 表达式	类型	                值（地址）	   +1 的偏移量
// a	int*（退化为指针）	        &a[0]	       sizeof(int)
// &a	int (*)[5]（指向数组的指针  &a[0]	       sizeof(a)（整个数组的大小）


// 在大多数表达式中，数组名 a 会隐式转换为指向首元素的指针。
// 但有两个例外：

// sizeof(a) 操作

// &a 操作（取地址）

// 在这两个场景中，a 仍然保持为“数组类型”，不会退化为指针。

    // int a[5] = {10, 20, 30, 40, 50};
    
    // cout << *(a + 1) << endl;   // 输出 20



// 拷贝构造函数（Copy Constructor）
// 触发时机：对象被创建时，用另一个同类型对象来初始化它。


class MyClass0812 {
public:
    // 拷贝构造函数的典型签名
    MyClass0812(const MyClass0812& other) {
        // 执行深拷贝或其他初始化逻辑
    }
};

int main0812_2() {
    MyClass0812 obj1;
    MyClass0812 obj2 = obj1;   // ✅ 调用拷贝构造函数（obj2 刚被创建）
    MyClass0812 obj3(obj1);    // ✅ 调用拷贝构造函数（obj3 刚被创建）
    return 0;
}
// 何时会调用拷贝构造？

// 用一个对象初始化另一个对象（如 obj2 = obj1 在定义时）。

// 函数按值传递对象（void func(MyClass obj)）。

// 函数按值返回对象（MyClass func()）。


// 赋值运算符重载（Copy Assignment Operator）
// 触发时机：两个对象都已经存在，将一个对象的值赋给另一个对象。

class MyClass0812_2 {
public:
    // 赋值运算符重载的典型签名
    MyClass0812_2& operator=(const MyClass0812_2& other) {
        if (this != &other) {   // 防止自赋值
            // 执行深拷贝或其他赋值逻辑
        }
        return *this;
    }
};

int main0812_2() {
    MyClass0812_2 obj1;
    MyClass0812_2 obj2;
    obj2 = obj1;   // ✅ 调用赋值运算符重载（obj2 已存在）
    return 0;
}
// 关键点：obj2 = obj1; 只有在 obj2 已经存在时，才会调用赋值运算符。
// 如果 obj2 是在这行代码中刚被定义，则会调用拷贝构造函数。




// 仿函数（Functor）和构造函数（Constructor）是C++中两个完全不同的概念，虽然都写在类里面，但它们在目的、调用时机和语法形式上有本质区别。

// 为了让你一眼看清，我先用一个表格总结它们的核心差异：

// 对比维度	仿函数（Functor）	构造函数（Constructor）
// 核心目的	让对象可以像普通函数一样被调用	初始化类的对象（分配资源、设置初值）
// 特殊语法	重载 operator()	函数名与类名相同
// 调用时机	由你显式调用，想什么时候用就什么时候用	在对象创建时由编译器自动调用
// 被谁调用	被你的业务代码调用（如 f(3,4)）	被编译器在对象构造时调用
// 能否被重载	可以，operator() 的参数或返回值类型可以不同	可以，但必须满足构造函数的特点（无返回值、函数名与类名相同）
// 调用次数	一个对象的生命周期内可以被多次调用	在对象的生命周期中只调用一次（构造时）
// 🔍 举个例子，更直观

class Functor_0812 {
public:
    // 这是构造函数：对象创建时调用
    Functor_0812() {
        std::cout << "构造函数被调用" << std::endl;
    }

    // 这是仿函数：对象像函数一样被调用时触发
    int operator()(int x, int y) {
        return x + y;
    }
};

int main0812_3() {
    Functor_0812 f;          // ✅ 构造函数被调用（对象创建时）
    int result = f(3,4); // ✅ 仿函数被调用（显式调用）
    int result2 = f(5,6); // ✅ 仿函数再次被调用（可以多次）
    return 0;
}
//仿函数作用，构造函数没有单参构造函数，单仍然可以给类对象传参