#include <iostream>
#include <map>
#include "vers.h"
#include "myallocator.h"
#include "mycontainers.h"

template<typename T, typename Alloc, typename K, typename CMP_F>
std::ostream& operator<<( std::ostream& os, const std::map<K, T, CMP_F, Alloc>& cont)
{
	for(const auto& el: cont)
		os << std::dec << el.first << " " << el.second << std::endl;	
	return os;
}

template<typename T, typename Alloc>
std::ostream& operator<<( std::ostream& os, const otus_hw3::MyList<T, Alloc>& cont)
{
	for(const auto& el: cont)
		os << std::dec << el << std::endl;	
	return os;
}

int main(int, char **) 
{
	constexpr int N_MAX = 10;
	using namespace otus_hw3;
	try
	{
		auto fact = [](int N) -> int {
						int F = 1;
						for(; N > 1 ; --N) F = F * N;
						return F;  
					};

		auto m0 = std::map<int, int>{};
		for(int i = 0; i < N_MAX; ++i)
			m0[i] = fact(i);

		auto m1 = std::map<int, int, std::less<int>, std_11_pool_allocator<int, static_cast<size_t>(N_MAX), 1>>{};
		for(int i = 0; i < N_MAX; ++i)
			m1[i] = fact(i);
		std::cout << m1;

		auto L0 = MyList<int, std::allocator<int>>{};
		for(int i = 0; i < N_MAX; ++i)
			L0.push_back(i);					

		auto L1 = MyList<int, std_11_pool_allocator<int, static_cast<size_t>(N_MAX), 2> >{};
			for(int i = 0; i < N_MAX; ++i)
				L1.push_back(i);
		std::cout << L1;						
	}	
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
