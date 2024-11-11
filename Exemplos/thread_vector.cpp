#include <iostream>
#include <thread>
#include <vector>
#include <mutex>


using namespace std;

mutex m;

void f(int i)
{
	m.lock();
	cout << "Thread criada - Id: " << i << endl;
	m.unlock();
	this_thread::sleep_for(1s);
}

int main()
{
	int num_threads;
	cout << "Entre com o numero de threads: ";
	cin >> num_threads;

	vector<thread> thread_vec;
	for (int i =0; i < num_threads; ++i)
	{
		thread_vec.push_back(thread(f,i));
	}
	for (int i=0; i < num_threads; ++i)
	{
		thread_vec[i].join();
	}
	return(0);
}
