#include <iostream>


class my {
    std::string t;
    public:
    // my(){
    //     t = "my1";
    // }
    my(std::string name = "my"): t(name){}
};


int main()
{
    my *a = (my*)malloc(sizeof(my));

    //a+= 14;
    system("leaks a.out");
    return 0;
}