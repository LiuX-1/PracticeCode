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

//虚函数统一放在虚函数表，表中存着函数指针作为函数参数，通过函数指针找到函数入口地址，回调函数，执行我写的代码。多态的本质。


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
    return 0;
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
    return 0;
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

// 静态成员函数中，不能使用普通变量。
// 	//静态成员变量属于整个类的，分不清楚，是那个具体对象的属性。



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
    return 0;
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
    MyClass0812() {
        // 默认构造函数
    }
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

int main0812_2_sub() {
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
//仿函数作用，构造函数没有带参构造函数，单仍然可以给类对象传参



// 组合与继承优缺点？
// 一：继承 继承是Is a 的关系，比如说Student继承Person,则说明Student is a Person。
// 继承的优点是子类可以重写父类的方法来方便地实现对父类的扩展。 
// 继承的缺点有以下几点： ①：父类的内部细节对子类是可见的。 
// ②：子类从父类继承的方法在编译时就确定下来了，所以无法在运行期间改变从父类继承的方法的行为。
//  ③：如果对父类的方法做了修改的话（比如增加了一个参数），则子类的方法必须做出相应的修改。
//  所以说子类与父类是一种高耦合，违背了面向对象思想。 
//  二：组合 组合也就是设计类的时候把要组合的类的对象加入到该类中作为自己的成员变量。 
//  组合的优点： ①：当前对象只能通过所包含的那个对象去调用其方法，
//  所以所包含的对象的内部细节对当前对象时不可见的。 
//  ②：当前对象与包含的对象是一个低耦合关系，如果修改包含对象的类中代码不需要修改当前对象类的代码。
//   ③：当前对象可以在运行时动态的绑定所包含的对象。可以通过set方法给所包含对象赋值。 
//   组合的缺点：①：容易产生过多的对象。②：为了能组合多个对象，必须仔细对接口进行定义。


// 优缺点对比（结合你的笔记）
// 对比维度	继承（Inheritance）   	               组合（Composition）
// 关系类型	纵向的“Is-A”	                       横向的“Has-A”
// 代码复用方式	子类自动拥有父类的所有成员和方法	通过包含的对象调用其功能
// 封装性	❌ 破坏封装，子类知道父类的内部细节      	✅ 被包含对象的内部细节对当前类不可见
// 耦合度	❌ 高耦合。父类改动会直接影响子类	     ✅ 低耦合。被包含的类改动不影响当前类
// 运行时灵活性	❌ 编译时确定，无法在运行时改变父类方法的行为	✅ 运行时可以动态替换所包含的对象（通过 set 方法）
// 修改影响范围	❌ 父类的方法签名变化，所有子类都要改	✅ 只要接口不变，修改被包含类的内部实现不影响调用方
// 对象数量	✅ 不会产生额外的对象	            ❌ 可能会产生较多的对象（被包含的对象）
// 接口设计难度	✅ 相对简单	                    ❌ 需要仔细设计接口，以方便组合多个对象



// lambda表达式
// Lambda 函数是 C++11 引入的一个特性，它允许你在代码中就地定义匿名函数，而不需要单独写一个函数或仿函数类。
// 它特别适合用在那些只需要临时使用、一次性的函数场景中，比如作为算法的比较器。

// [capture](parameters) -> return_type { body }

// int x = 10;
// auto func = [&x]() {
//     x = 20;              // ✅ 修改外部的 x
// };
// func();
// cout << x << endl;       // 输出 20
// 写法	含义
// [x]	值捕获，复制一份，只读
// [&x]	引用捕获，可以修改外部变量
// [=]	所有用到的外部变量都值捕获
// [&]	所有用到的外部变量都引用捕获


// 最常用的场景：作为算法的参数
// Lambda 最常见的用法是传给 STL 算法，比如 sort：

// cpp
// vector<int> v = {3, 1, 4};

// // 用 lambda 作为排序规则，降序排列
// sort(v.begin(), v.end(), [](int a, int b) {
//     return a > b;
// });
// 这个 lambda 不需要名字，不需要单独定义函数，直接写在 sort 的参数里，用完即走。
// 这就是 lambda 存在的意义——让临时用的函数能就地写，不污染命名空间。


// 在 C++11 之前，只能用仿函数
// 在 C++11 标准之前，如果你想给 sort 传一个自定义的排序规则，只能定义一个仿函数（或者函数指针）：

// cpp
// // 必须单独定义一个类（即使只用一次）
// struct MyComparator {
//     bool operator()(int a, int b) const {
//         return a > b;
//     }
// };

// int main() {
//     vector<int> v = {3, 1, 4};
//     sort(v.begin(), v.end(), MyComparator()); // 传一个临时对象
// }



// 关联、聚合、组合概念

// 三者对比总结
// 对比维度	关联（Association）	聚合（Aggregation）	组合（Composition）
// 关系强弱	最弱	中等	最强
// 生命周期绑定	完全独立	部分可独立于整体存在	部分的生命周期由整体严格管理
// 整体销毁时，部分是否销毁？	不影响	不影响（部分继续存在）	✅ 部分随之销毁
// UML 表示法	实线箭头（无菱形）	空心菱形箭头	实心菱形箭头
// 典型实现方式	方法参数、局部变量	指针或引用成员（外部传入）	成员变量（值对象）
// 例子	医生 - 病人	汽车 - 轮子	汽车 - 引擎


// 记住两句话：

// “病人不是医生的附属品” → 关联

// “轮子可以拆下来给别的车用” → 聚合

// “引擎是车的一部分，车没了引擎也没了” → 组合



// 这三个概念和继承（Is-A）是平行的：

// 继承 = Is-A（学生是人）

// 关联/聚合/组合 = Has-A（汽车有引擎）


//简单记就是，关联是参数相关，聚合是指针相关，组合是对象相关。

// 在实际设计中，优先考虑组合/聚合，而不是继承，因为继承会带来更高的耦合度。
// 而组合和聚合中，优先选择组合还是聚合，则取决于部分是否需要脱离整体独立存在。



// 简述数组与指针的区别？
int funtest(char * p) {
    cout << sizeof(p) << endl;  // 输出 8（在64位系统上，指针大小为8字节）
    return 0;
}

int main0813() {
    char a[10] = "hello";
    char * p = a;
    cout << sizeof(a) << endl;  // 输出 10
    cout << sizeof(p) << endl;  // 输出 8（在64位系 统上，指针大小为8字节）
    funtest(p);
    return 0;
}

//简单说就是，sizeof数组名，则拿到的是数组大小。 如果通过函数参数将数组名传进来然后sizeof这个指针，则拿到的是指针大小。因为数组名在函数参数中会退化为指针类型。





// 设计模式

// 创建型模式--单例模式
// 单例模式是一种对象创建型模式，使用单例模式，可以保证为一个类只生成唯一的实例对象。
// 也就是说，在整个程序空间中，该类只存在一个实例对象。

// 实现单例步骤常用步骤   
// a)	构造函数私有化
// b)	提供一个全局的静态方法（全局访问点）
// c)	在类中定义一个静态指针，指向本类的变量的静态变量指针




//同步异步 阻塞非阻塞

// 同步，就是我调用一个功能，该功能没有结束前，我死等结果。

// 异步，就是我调用一个功能，不需要知道该功能结果，该功能有结果后通知我（回调通知）

// 阻塞，就是调用我（函数），我（函数）没有接收完数据或者没有得到结果之前，我不会返回。

// 非阻塞，就是调用我（函数），我（函数）立即返回，通过select通知调用者

// 同步IO和异步IO的区别就在于：数据拷贝的时候进程是否阻塞

// 阻塞IO和非阻塞IO的区别就在于：应用程序的调用是否立即返回


//移动构造函数

// 如果类定义了移动构造函数 → 编译器会优先调用它。

// 如果类没有定义移动构造函数 → 编译器不会“默认生成一个”，而是会退而求其次，选择拷贝构造函数（如果存在的话）。

// 三种情况对比

// 情况一：定义了移动构造
// cpp
// class MyClass {
// public:
//     MyClass(MyClass&&) { cout << "移动构造" << endl; }
//     MyClass(const MyClass&) { cout << "拷贝构造" << endl; }
// };

// MyClass a;
// MyClass b = std::move(a);   // ✅ 输出：移动构造


// 情况二：没定义移动构造，但定义了拷贝构造
// cpp
// class MyClass {
// public:
//     MyClass(const MyClass&) { cout << "拷贝构造" << endl; }
//     // 没有移动构造
// };

// MyClass a;
// MyClass b = std::move(a);   // ✅ 输出：拷贝构造（退而求其次）
// 即使你用了 std::move，编译器也找不到移动构造，只能用拷贝构造替代。


// 情况三：既没定义移动构造，也没定义拷贝构造
// cpp
// class MyClass {
//     // 什么构造都没写
// };

// MyClass a;
// MyClass b = std::move(a);   // ✅ 编译器生成默认的拷贝构造（逐成员复制）


// 只有在特定条件下，编译器才会自动生成移动构造函数：

// 类没有显式定义拷贝构造函数、拷贝赋值运算符、移动赋值运算符和析构函数中的任何一个。

// 并且类的所有成员都是可移动的（比如 std::string、std::vector）。


// 总结
// 情况	编译器行为
// 定义了移动构造	用移动构造（✅ 优先）
// 没定义移动构造，但有拷贝构造	用拷贝构造（退而求其次）
// 都没定义，且满足自动生成条件	编译器自动生成移动构造（浅拷贝行为）
// 都没定义，且不满足自动生成条件	用拷贝构造（如果有）或编译报错


// 那 std::move 在初始化中必须用吗？
// 不一定。
// cpp
// MyClass func() {
//     return MyClass();   // 返回临时对象，自动触发移动构造（不需要 std::move）
// }
// MyClass b = func();     // 自动触发移动构造（或 RVO 优化）

// 注意，如果类没有定义移动构造函数，编译器不会自动生成一个并调用它，而是会退而求其次，调用拷贝构造函数（如果存在的话）。



//单例模式- 懒汉式

//懒汉式
class Singelton
{
private:
	Singelton()
	{
		cout << "Singelton 构造函数执行" << endl;
	}
public:
	static Singelton *getInstance()
	{
		if (m_psl == NULL)
		{
			m_psl = new Singelton;
		}
		return m_psl;
	}

	static void FreeInstance()
	{
		if (m_psl != NULL)
		{
			delete m_psl;
			m_psl = NULL; 
		}
	}

private:
	static Singelton *m_psl;
};

Singelton *Singelton::m_psl = NULL;


void main041()
{
	
	Singelton *p1 = Singelton::getInstance();
	Singelton *p2 = Singelton::getInstance();

	if (p1 == p2)
	{
		cout << "是同一个对象" << endl;
	}
	else
	{
		cout << "不是同一个对象" << endl;
	}
	Singelton::FreeInstance();

	
	return ;
}





//饿汉式
class Singelton
{
private:
	Singelton()
	{
		cout << "Singelton 构造函数执行" << endl;
	}
public:
	static Singelton *getInstance()
	{
		return m_psl;
	}

	static void FreeInstance()
	{
		if (m_psl != NULL)
		{
			delete m_psl;
			m_psl = NULL; 
		}
	}

private:
	static Singelton *m_psl;
};

//int g_count = 0;
//饿汉式
Singelton *Singelton::m_psl = new Singelton;


void main041()
{
	printf("sss\n");
	Singelton *p1 = Singelton::getInstance();
	Singelton *p2 = Singelton::getInstance();

	if (p1 == p2)
	{
		cout << "是同一个对象" << endl;
	}
	else
	{
		cout << "不是同一个对象" << endl;
	}
	Singelton::FreeInstance();

	return ;
}

//这里的 instance 是一个静态成员变量。它的初始化会在 main 函数开始之前完成，且只执行一次。

// 所以，当你的程序进入 main() 时，这个唯一的 Singleton 对象已经静静地躺在内存里了。



//单例模式    不用new   懒汉模式

class Singleton {
private:
    Singleton() = default;                 // 私有构造
    ~Singleton() = default;                // 私有析构（可选）
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    static Singleton& getInstance() {
        static Singleton instance;          // 核心：函数内的静态变量
        return instance;
    }

    void doSomething() const {
        // ...
    }
};

int main0819() {
    Singleton& s = Singleton::getInstance();
    s.doSomething();
}



//简单工厂模式

class Fruit 
{
public:
	virtual void getFruit() = 0;

protected:
private:
};

class Banana : public Fruit
{
public:
	virtual void getFruit()
	{
		cout << "我是香蕉...." << endl;
	}
protected:
private:
};

class Apple : public Fruit
{
public:
	virtual void getFruit()
	{
		cout << "我是苹果...." << endl;
	}
protected:
private:
};


class Factory
{
public:
	Fruit *create(char *p)
	{

		if (strcmp(p, "banana") == 0)
		{
			return new Banana;	 
		}
		else if (strcmp(p, "apple") == 0)
		{
			return new Apple;
		}
		else
		{
			printf("不支持\n" ) ;
			return NULL;
		}
	}
};


void main0820()
{
	Factory *f = new Factory;

	Fruit *fruit = NULL;


	//工厂生产 香蕉
	fruit = f->create("banana");
	fruit->getFruit();
	delete fruit;


	fruit = f->create("apple");
	fruit->getFruit();
	delete fruit;

	delete f;
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}




class Fruit
{
public:
	virtual void  sayname()  = 0;
};

class  Banana : public Fruit
{
public:
	void  sayname()
	{
		cout << "我是香蕉" << endl;
	}
};

class  Apple : public Fruit
{
public:
	void  sayname()
	{
		cout << "我是 Apple" << endl;
	} 
};

class  AbFactory 
{
public:
	virtual Fruit *CreateProduct() = 0;
};

class BananaFactory :public AbFactory
{
public:
	virtual Fruit *CreateProduct()
	{
		return new Banana;
	}
};

class AppleFactory :public AbFactory
{
public:
	virtual Fruit *CreateProduct()
	{
		return new Apple;
	}
};

//////////////////////////////////////////////////////////////////////////
//添加新的产品

class Pear : public Fruit
{
public:
	virtual void sayname()
	{
		cout << "我是 pear" << endl;
	}
protected:
private:
};

class PearFactory : public AbFactory
{
public:
	virtual Fruit *CreateProduct()
	{
		return new Pear;
	}
};



void main0820_1()
{
	AbFactory		*factory = NULL;
	Fruit			*fruit = NULL;

	//吃 香蕉
	factory = new BananaFactory;
	fruit = factory->CreateProduct();   //多态，父类指针指向子类对象，然后下一步这个指针调用子类的sayname()方法
	fruit->sayname();

	delete fruit;
	delete factory;


	//Pear 
	factory = new PearFactory;
	fruit = factory->CreateProduct();
	fruit->sayname();

	delete fruit;
	delete factory;

	
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}





class Fruit
{
public:
	virtual void SayName() = 0;
};

class AbstractFactory
{
public:
	virtual Fruit* CreateBanana() = 0;
	virtual Fruit* CreateApple() = 0;
};

class NorthBanana : public Fruit
{
public:
	virtual void SayName()
	{
		cout << "我是北方香蕉" << endl;
	}
};

class NorthApple : public Fruit
{
public:
	virtual void SayName()
	{
		cout << "我是北方苹果" << endl;
	}
};


class SourthBanana : public Fruit
{
public:
	virtual void SayName()
	{
		cout << "我是南方香蕉" << endl;
	}
};


class SourthApple : public Fruit
{
public:
	virtual void SayName()
	{
		cout << "我是南方苹果" << endl;
	}
};

class NorthFacorty : public AbstractFactory
{
	virtual Fruit * CreateBanana()
	{
		return new NorthBanana;
	}
	virtual Fruit * CreateApple()
	{
		return new NorthApple;
	}
};

class SourthFacorty : public AbstractFactory
{
	virtual Fruit * CreateBanana()
	{
		return new SourthBanana;
	}
	virtual Fruit * CreateApple()
	{
		return new SourthApple;
	}
};


void main()
{
	Fruit			*fruit = NULL;
	AbstractFactory *af = NULL;

	///--------------
	af = new SourthFacorty;
	fruit = af->CreateApple();
	fruit->SayName();
	delete fruit;
	fruit = af->CreateBanana();
	fruit->SayName();
	delete fruit;

	///------
	af = new NorthFacorty;
	fruit = af->CreateApple();
	fruit->SayName();
	delete fruit;
	fruit = af->CreateBanana();
	fruit->SayName();
	delete fruit;

	delete af;
	system("pause");
	return ;
}



//设计模式比较高频的

常见度总结
模式	考察频率	所属类型
单例模式	⭐⭐⭐⭐⭐	创建型
工厂模式	⭐⭐⭐⭐	创建型
观察者模式	⭐⭐⭐⭐	行为型
策略模式	⭐⭐⭐⭐	行为型
装饰器模式	⭐⭐⭐	结构型
适配器模式	⭐⭐⭐	结构型
模板方法	⭐⭐⭐	行为型
建造者模式	⭐⭐	创建型
代理模式	⭐⭐	结构型



//结构型模式， 代理模式
class Subject
{
public:
	virtual void sailbook() = 0;
};

class RealSubjectBook : public Subject
{
public:
	virtual void sailbook()
	{
		cout << "卖书" << endl;
	}
};

//a中包含b类；a、b类实现协议类protocol 
class dangdangProxy : public Subject
{
public:
	virtual void sailbook()
	{
		RealSubjectBook *rsb = new RealSubjectBook;
		dazhe();
		rsb->sailbook();
		dazhe();
	}
public:
	void dazhe()
	{
		cout << "双十一打折" << endl;
	}
private:
	Subject *m_subject;
};

void main()
{
	Subject *s = new dangdangProxy;
	s->sailbook();
	delete s;
	system("pause");
	return ;
}


//子类中的拷贝构造函数
//子类对象初始化父类对象
class Base {
private:
    int i;
public:
    Base(int val) : i(val) {}
    Base(const Base& other) : i(other.i) {}   // 基类拷贝构造
};

class Derived : public Base {
private:
    int j;
public:
    Derived(int a, int b) : Base(a), j(b) {}
    
    // 拷贝构造：用 obj 的 Base 部分初始化 Base，用 obj.j 初始化 j
    Derived(const Derived& obj) : Base(obj), j(obj.j) {}      //用子类初始化基类
};


//基类不能直接初始化子类
class Base {
private:
    int i;
public:
    Base(int val) : i(val) {}
};

class Derived : public Base {
private:
    int j;
public:
    Derived(int a, int b) : Base(a), j(b) {}
};

int main() {
    Derived d1(1, 2);
    Base b1 = d1;          // ✅ 可以：用子类给基类赋值（切片）
    
    Base b2(10);
    // Derived d2 = b2;    // ❌ 编译错误：无法用基类对象初始化子类
    // Derived d3(b2);     // ❌ 编译错误：同样不行
    return 0;
}



//结构型模式---装饰模式

class Car
{
public:
	virtual void show() = 0;
};

class RunCar : public Car 
{
public:
	virtual void show()
	{
		cout << "可以跑" << endl;
	}
protected:
private:
};


class SwimCarDirector : public Car
{
public:
	SwimCarDirector(Car *car)
	{
		m_car = car;
	}
	void swim()
	{
		cout << "可以游" << endl;
	}
	virtual void show()
	{
		m_car->show();
		swim();
	}
protected:
private:
	Car *m_car;    //装饰器都有一个基类的指针成员，指向被装饰对象flycar
};

class  FlyCarDirector : public Car
{
public:
	FlyCarDirector(Car *car)
	{
		m_car = car;
	}
	void fly()
	{
		cout << "可以飞" << endl;
	}
	virtual void show()
	{
		m_car->show();    //父类指针m_car调用虚函数show，实际调用的是子类RunCar的show方法
		fly();            //增加装饰类的功能
	}

private:
	Car *m_car;    //has-a 关系
};

void main()    //这个例子跟下面那个装饰模式列子的一个缺少的功能点是，没有实现一个装饰类继承另一个装饰类的功能。 也就是没有实现一环套一环的功能。 下面那个例子实现了这个功能。
{                                                                //上面这句话不对，下面那个例子只是在实现接口，跟上面这个例子本质还是一样的。下面那个没有做一个装饰类继承另一个装饰类的事。
	Car * mycar = NULL;
	mycar = new RunCar;
	printf("-----000\n");
	mycar->show();

	printf("-----aaa\n");

	FlyCarDirector *flycar = new FlyCarDirector(mycar);     //has-a 关系， 除了可以run还能fly
	flycar->show();

	printf("-----bbb\n");
	SwimCarDirector *swimcar = new SwimCarDirector(flycar);
	swimcar->show();    //此处输出run fly swim，说明一环套一环的功能实现了。 也就是装饰类继承另一个装饰类的功能实现了。 
	
	delete swimcar;
	delete flycar;
	delete mycar;
	system("pause");
	return ;
}

//上下这两个例子都能实现一环套一环的功能，区别是上面那个例子没有实现装饰类继承另一个装饰类的功能（重新看了下，这句话描述不对），下面这个例子实现了这个功能。 也就是下面这个例子更符合装饰模式的定义。

// 关键点（面试回答的核心）
// 面试官问“装饰模式的关键点”时，通常期望你答出以下三点：

// 动态组合，而非静态继承

// 继承是在编译时确定的，而装饰器是在运行时组合的。你可以灵活地叠加多个装饰器，顺序也可以调整。

// 例如：new EncryptedStream(new BufferedStream(new FileStream("a.txt"))) 同时具备缓冲和加密功能。

// 透明性

// 装饰器和被装饰对象继承自同一个抽象接口，客户代码无需区分是原始对象还是被装饰过的对象。

// 对客户端来说，调用方式完全一致。

// 符合开闭原则

// 新增功能无需修改现有类，只需新增一个装饰器类即可。这是“对扩展开放，对修改关闭”的典型体现。



//继续看装饰模式

// 抽象组件：定义读取/写入数据的接口
class Stream {
public:
    virtual void write(const string& data) = 0;
    virtual ~Stream() = default;
};

// 具体组件：核心功能（文件流）
class FileStream : public Stream {
public:
    void write(const string& data) override {
        cout << "写入文件: " << data << endl;
    }
};

// 装饰器基类：持有一个指向抽象组件的指针
class StreamDecorator : public Stream {
protected:
    Stream* stream;       //装饰器都有一个基类的指针成员，指向被装饰对象
public:
    StreamDecorator(Stream* s) : stream(s) {}     //初始化列表，相当于Stream* stream = file
    void write(const string& data) override {
        stream->write(data);   // 委托给被装饰对象       //父类指针stream调用虚函数write，实际调用的是子类FileStream的write方法
    }
};

// 具体装饰器A：加密功能
class EncryptedStream : public StreamDecorator {
public:
    EncryptedStream(Stream* s) : StreamDecorator(s) {}
    void write(const string& data) override {
        string encrypted = "加密(" + data + ")";
        stream->write(encrypted);   // 先加密，再写入   //此处也是多态，父类指针stream调用虚函数
    }
};    //具体装饰器 不包含stream成员。 通过Stream* s父类指针，指向子类对象buffered，实现对StreamDecorator的实例化。
      // 然后EncryptedStream类的write方法调用父类StreamDecorator的write方法，传入加密后的数据，实现了对原始写入功能的增强。

// 具体装饰器B：缓冲功能
class BufferedStream : public StreamDecorator {
public:
    BufferedStream(Stream* s) : StreamDecorator(s) {}
    void write(const string& data) override {
        string buffered = "缓冲(" + data + ")";
        stream->write(buffered);
    }
};

// 客户端代码
int main() {
    FileStream file;
    
    // 只加缓冲
    BufferedStream buffered(&file);
    buffered.write("Hello");   // 输出: 写入文件: 缓冲(Hello)
    
    // 缓冲 + 加密
    EncryptedStream encrypted(&buffered);
    encrypted.write("World");  // 输出: 写入文件: 缓冲(加密(World))
    
    return 0;
}


// 所以更准确的理解是
// 时刻	发生的事情	说明
// 构造时	Stream* s = &buffered;	父类指针指向子类对象（你指出的那点）
// 调用时	stream->write(...)	用这个父类指针调用虚函数（多态的实际表现）
// 构造时是指针的“赋值时刻”，调用时是指针的“使用时刻”。
//    两者是同一个指针在不同阶段的表现，共同构成了装饰模式中多态的基础。

// “一环套一环”，这恰恰是装饰模式（Decorator Pattern）最精髓，也最让人头疼的地方。


// 自己的理解，装饰模式，就跟套娃一样，一层包一层，下一层基于已有的上一层做包装。 然后如果直接实例化最外边一层，里面的也就都会有了。

// 你理解得对的地方
// “一层包一层” → 装饰器通过组合，把多个功能层叠在一起。

// “下一层基于已有的上一层做包装” → 每个装饰器在调用内层对象的基础上，添加自己的功能。

// “实例化最外边一层，里面的也就都有了” → 只需要持有最外层的对象，调用它的方法时，整条链会自动执行。

// 你用“套娃”来理解装饰模式，说明你已经抓住了它的本质：层层包装、逐层委托、灵活组合。这个理解比很多只是背概念的人要深刻得多。


//适配器模式
// 适配器模式（Adapter Pattern）通过引入一个适配器类，将一个类的接口转换成客户端期望的另一个接口，从而解决接口不兼容的问题。
// 形象比喻：你有一台两孔插头的电器，但墙上只有三孔插座。适配器（转换插头）把三孔转成两孔，电器就能正常工作了。
// 代码比喻：你有一个老旧的日志库，它的接口是 writeLog(msg)，但你的新系统期望的是 log(msg)。适配器把 log 调用转换成 writeLog 调用。

// 被适配者：旧日志库（不兼容的接口）
class OldLogger {
public:
    void writeLog(const string& msg) {
        cout << "[旧日志] " << msg << endl;
    }
};

// 目标接口：新系统期望的日志接口
class Logger {
public:
    virtual void log(const string& msg) = 0;
    virtual ~Logger() = default;
};

// 适配器：把 OldLogger 适配成 Logger 接口
class LoggerAdapter : public Logger {   //基类是目标接口，子类是适配器类
private:
    OldLogger* oldLogger;
public:
    LoggerAdapter(OldLogger* old) : oldLogger(old) {}    // 构造函数接受被适配者的实例

    void log(const string& msg) override {
        oldLogger->writeLog(msg);   // 把 log 调用转换成 writeLog
    }
};

// 客户端代码（新系统）
int main() {
    OldLogger old;
    Logger* logger = new LoggerAdapter(&old);
    logger->log("系统启动成功");     // 输出: [旧日志] 系统启动成功
    delete logger;
    return 0;
}



// 适配器模式 视频
class Current18v
{
public:
	void use18vCurrent()
	{
		cout << "使用18v的交流电" << endl;
	}
protected:
private:
};


class Current220v
{
public:
	void use220vCurrent()
	{
		cout << "使用220v的交流电" << endl;
	}
protected:
private:
};


class Adapter : public Current18v    // 基类是目标接口，子类是适配器类
{
public:
	Adapter(Current220v *p220v)      // 构造函数接受被适配者的实例
	{
		m_p220v = p220v;
	}
	void use18vCurrent()
	{
		cout << "adapter中使用电流" << endl;
		m_p220v->use220vCurrent();
	}
protected:
private:
	Current220v *m_p220v;
};

void main()
{
	Current220v *p220v = new Current220v;
	Adapter *padapter = new Adapter(p220v);
	padapter->use18vCurrent();

	delete p220v;
	delete padapter;
	system("pause");
	return ;
}



//行为模式

//行为型--模板模式
class MakeCar
{
public:
	virtual void MakeHead() = 0;
	virtual void MakeBody() = 0;
	virtual void MakeTail() = 0;

public:
	void Make() //模板函数 把业务逻辑给做好
	{
		MakeTail();
		MakeBody();
		MakeHead();
	}
};

//
class Jeep : public MakeCar
{
public:
	virtual void MakeHead()
	{
		cout << "jeep head" << endl;
	}

	virtual void MakeBody()
	{
		cout << "jeep body" << endl;
	}

	virtual void MakeTail()
	{
		cout << "jeep tail" << endl;
	}
};

class Bus : public MakeCar
{
public:
	virtual void MakeHead()
	{
		cout << "Bus head" << endl;
	}

	virtual void MakeBody()
	{
		cout << "Bus body" << endl;
	}

	virtual void MakeTail()
	{
		cout << "Bus tail" << endl;
	}
};

void main()
{
	MakeCar *car = new Bus;
	car->Make();
	delete car;

	MakeCar *car2 = new Jeep;
	car2->Make();
	delete car2;

	
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}

//car->Make() 调用的不是子类的 Make 函数（因为子类确实没有重写它），
// 而是基类 MakeCar 中定义的 Make 函数。 但这个基类的 Make 函数内部，
// 又调用了三个虚函数（MakeHead、MakeBody、MakeTail），
// 这些虚函数在子类中被重写了，因此表现出了多态行为。



//行为型--命令模式

class Doctor
{
public:
	void treat_eye()
	{
		cout << "医生 治疗 眼科病" << endl;
	}

	void treat_nose()
	{
		cout << "医生 治疗 鼻科病" << endl;
	}
};


class CommandTreatEye
{
public:
	CommandTreatEye(Doctor *doctor)
	{
		m_doctor = doctor;
	}
	void treat()
	{
		m_doctor->treat_eye();
	}
private:
	Doctor *m_doctor;
};


class CommandTreatNose
{
public:
	CommandTreatNose(Doctor *doctor)
	{
		m_doctor = doctor;
	}
	void treat()
	{
		m_doctor->treat_nose();
	}
private:
	Doctor *m_doctor;
};



void main21_1()
{
	//1 医生直接看病
	/*
	Doctor *doctor = new Doctor ;
	doctor->treat_eye();
	delete doctor;
	*/

	//2 通过一个命令 医生通过 命令 治疗 治病
	Doctor *doctor = new Doctor ;
	CommandTreatEye * commandtreateye = new CommandTreatEye(doctor); //shift +u //转小写 shift+ctl + u转大写  
    //意义在于，医生不直接看病，而是通过命令来治疗病人。
    // 也就是医生和病人之间有一个中间层，医生不直接接触病人，而是通过命令来治疗病人。
    // 这样做的好处是，医生和病人之间的耦合度降低了。 也就是医生和病人之间的关系变得松散了。 医生和病人之间的关系变得灵活了。 医生和病人之间的关系变得可扩展了。
	commandtreateye->treat();
	delete commandtreateye;
	delete doctor;
	return ;
}
//方式1方式2对比：

// 方式1，如果将来：
// treat_eye() 改名为 treat_ophthalmology()
// treat_eye() 需要增加一个参数
// 治疗逻辑从医生转移到了护士或AI系统
// 那么所有调用 doctor->treat_eye() 的地方，都需要逐一修改。这就是紧耦合——客户端和 Doctor 的具体实现绑死了。

// 方式2，好处在于：
// 客户端依赖的是接口，而不是具体方法
// 所有命令都提供统一的 treat() 接口。
// 客户端不关心命令内部是调用 treat_eye 还是 treat_nose，只需要 treat() 就行了。

// 变更可以局部化
// 如果 treat_eye() 改名了，只需要修改 CommandTreatEye 这一个类，所有使用该命令的客户端代码都不需要改动。
// 如果需要改变治疗流程（比如先检查再治疗），也只需要修改命令类内部的实现，不影响客户端。

// 扩展性更强
// 可以通过配置文件或运行时条件，决定创建哪个命令对象，而不是在代码中硬编码调用哪个方法。

void main()
{
	main21_1();
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}


//命令模式-2

class Doctor
{
public:
	void treat_eye()
	{
		cout << "医生 治疗 眼科病" << endl;
	}

	void treat_nose()
	{
		cout << "医生 治疗 鼻科病" << endl;
	}
};

class Command
{
public:
	virtual void treat() = 0;
};
class CommandTreatEye : public Command  //基于抽象类派生子类，用来实现对不同命令的封装。里面包含执行命令的Doctor类。
{
public:
	CommandTreatEye(Doctor *doctor)
	{
		m_doctor = doctor;
	}
	void treat()
	{
		m_doctor->treat_eye();
	}
private:
	Doctor *m_doctor;    // 包含Doctor，通过这种方式，实现command和Doctor的解耦。
};


class CommandTreatNose : public Command
{
public:
	CommandTreatNose(Doctor *doctor)
	{
		m_doctor = doctor;
	}
	void treat()
	{
		m_doctor->treat_nose();
	}
private:
	Doctor *m_doctor;
};


class BeautyNurse
{
public:
	BeautyNurse(Command *command)
	{
		this->command = command;
	}
public:
	void SubmittedCase() //提交病例 下单命令
	{
		command->treat();
	}
protected:
private:
	Command *command;
};

//护士长
class HeadNurse 
{
public:
	HeadNurse()
	{
		m_list.clear();
	}
	
public:
	void setCommand(Command *command)
	{
		m_list.push_back(command);
	}
	void SubmittedCase() //提交病例 下单命令
	{
		for (list<Command *>::iterator it=m_list.begin(); it!=m_list.end(); it++)
		{
			(*it)->treat();
		}
	}
private:
	list<Command *> m_list;
};



void main21_1()
{
	//1 医生直接看病
	/*
	Doctor *doctor = new Doctor ;
	doctor->treat_eye();
	delete doctor;
	*/

	//2 通过一个命令 医生通过 命令 治疗 治病
	Doctor *doctor = new Doctor ;
	Command * command = new CommandTreatEye(doctor); //shift +u //转小写 shift+ctl + u转大写
	command->treat();   //多态。  提炼出Command抽象类，不同的命令类继承Command抽象类，重写treat()方法。 这样就实现了多态。 也就是医生通过命令来治疗病人。 也就是医生和病人之间有一个中间层，医生不直接接触病人，而是通过命令来治疗病人。
	delete command;
	delete doctor;
	return ;
}


void main21_2()
{
	//3 护士提交简历 给以上看病
	BeautyNurse		*beautynurse = NULL;
	Doctor			*doctor = NULL;
	Command			*command  = NULL;

	doctor = new Doctor ;

	//command = new CommandTreatEye(doctor); //shift +u //转小写 shift+ctl + u转大写
	command = new CommandTreatNose(doctor); //shift +u //转小写 shift+ctl + u转大写
	beautynurse = new BeautyNurse(command);
	beautynurse->SubmittedCase();
	
	delete doctor;
	delete command;
	delete beautynurse;
	return ;
}


//4 通过护士长 批量的下单命令
void main21_3()
{
	//护士提交简历 给以上看病
	HeadNurse		*headnurse = NULL;
	Doctor			*doctor = NULL;
	Command			*command1  = NULL;
	Command			*command2  = NULL;

	doctor = new Doctor ;

	command1 = new CommandTreatEye(doctor); //shift +u //转小写 shift+ctl + u转大写
	command2 = new CommandTreatNose(doctor); //shift +u //转小写 shift+ctl + u转大写

	headnurse = new HeadNurse(); //new 护士长
	headnurse->setCommand(command1);
	headnurse->setCommand(command2);

	headnurse->SubmittedCase(); // 护士长 批量下单命令

	delete doctor;
	delete command1;
	delete command2;
	delete headnurse;
	return ;
}


void main()
{
	//main21_1();
	//main21_2();
	main21_3();
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}


// 策略模式

class Strategy
{
public:
	virtual void crypt() = 0;
};

//对称加密  速度快 加密大数据块文件 特点:加密密钥和解密密钥是一样的.
//非对称加密 加密速度慢 加密强度高 高安全性高 ;特点: 加密密钥和解密密钥不一样  密钥对(公钥 和 私钥)
//


class AES :  public Strategy
{
public:
	virtual void crypt()
	{
		cout << "AES加密算法" << endl;
 	}
};
class DES :  public Strategy
{
public:
	virtual void crypt()
	{
		cout << "DES 加密算法" << endl;
	}
};


class Context
{
public:
	void setStrategy(Strategy *strategy)
	{
		this->strategy = strategy;
	}
	void myoperator()
	{
		strategy->crypt();
	}

protected:
private:
	Strategy *strategy;
};


void main()
{
	/*
	//1 
	DES *des = new DES;
	des->crypt();
	delete des;
	*/

	Strategy *strategy = NULL;

	//strategy = new DES;
	strategy = new AES;
	Context *context = new Context;
	context->setStrategy(strategy);
	context->myoperator();
	
	delete  strategy;
	delete  context;
		 
	cout<<"hello..."<<endl;
	system("pause");
	return ;
}