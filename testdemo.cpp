#include "Allocator.h"
#include"Alloc.h"
//#include<vector>
#include<iostream>
#include"Vector.h"
#include<vector>
using namespace std;
void test() {
	TokameinE::vector<int> s(1);
	s.push_back(1);
	s.push_back(2);
	s.push_back(3);
	s.push_back(4);

}


int main()
{
	
	test();

	vector<int> p(1,2);
	p.max_size();
	p.push_back(1);
	p.push_back(2);
	p.push_back(3);
	p.push_back(4);
	vector<int>::iterator f3 = p.begin();
	vector<int>::iterator f= p.erase(p.begin());
	vector<int>::iterator f2 = p.erase(p.end()-1);
	//char* p = (char*)TokameinE::alloc::allocate(32);
}


