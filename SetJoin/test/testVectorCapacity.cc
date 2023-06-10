#include <iostream>
#include <vector>

int main()
{
  using namespace std;
  
  vector<int> normal;
  cout << normal.capacity() << endl;
  
  for (unsigned int loop = 0; loop != 28; ++loop)
  {
      normal.push_back(1);
      cout << normal.capacity() << endl;
  }
  
  return 0;
}