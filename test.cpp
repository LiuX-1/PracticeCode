#include <iostream>
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

int main()
{
    cat c;
    dog d;
    test_animal(c);
    test_animal(d);
    return 0;
}