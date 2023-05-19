
#include "OvlpJoin.h"

int c;
vector<vector<unsigned short>> dataset;
vector<combination> combs;

/*
 * Recursive function to calculate binomial coefficient.
 *
 * The binomial coefficient, often referred to as "n choose k" or "n over k",
 * is the number of ways to choose k elements from a set of n elements 
 * without considering the order. The formula to calculate binomial coefficients is 
 * n! / (k!(n-k)!), where ! represents factorial.
 *
 * This function uses a recursive approach to calculate the binomial coefficient,
 * which simplifies the computation by avoiding the calculation of the full factorials.
 * This approach is based on the identity C(n, k) = n*C(n-1, k-1)/k.
 *
 * @param n The total number of elements in the set.
 * @param k The number of elements to choose from the set.
 * @return The binomial coefficient of n and k.
 */
int64_t nchoosek(int64_t n, int64_t k)
{
  if (k == 0)
    return 1;
  return (n * nchoosek(n - 1, k - 1)) / k;
}


// Function to handle the small sets
void OvlpJoin::small_case(int L, int R)
{

  // If the left boundary is greater or equal to the right boundary, return immediately
  if (L >= R)
    return;
  --c;

  cout << " number of small sets: " << R - L << endl;

  vector<vector<int>> res_lists;

  // Loop over all elements in reverse order
  for (auto idx = total_eles - 1; idx >= 0; idx--)
  {
    // cout<<"Current idx: "<<idx<<endl;
    if (ele_lists[idx].size() < 2)
      continue;
    vector<pair<int, int>> &vec = ele_lists[idx];
    int size = distance(vec.begin(), lower_bound(vec.begin(), vec.end(), L, comp_pair));
    if (vec.size() <= size + 1)
      continue;

    // initialize heap and combinations
    heap.clear();
    combs.clear();
    int heap_size = 0;
    for (auto i = size; i < vec.size(); i++)
    {
      if ((int)(dataset[vec[i].first].size()) - 1 - vec[i].second < c)
        continue;
      heap.push_back(heap_size++);
      combs.push_back(combination(vec[i].first, vec[i].second));
    }

    if (heap_size < 2)
      continue;
    make_heap(heap.begin(), heap.end(), comp_comb);
    // cout << heap_size << endl;

    // pop heaps
    vector<int> inv_list;
    while (heap_size > 1)
    {
      inv_list.clear();
      do
      {
        // cout << heap_size << " " << heap_cost << endl;
        pop_heap(heap.begin(), heap.begin() + heap_size, comp_comb);
        --heap_size;
        inv_list.push_back(combs[heap[heap_size]].id);
      } while (heap_size > 0 && is_equal(combs[heap[heap_size]], combs[heap.front()]));

      if (inv_list.size() > 1)
      {
        res_lists.push_back(std::move(inv_list));
      }

      if (heap_size == 0)
        break;

      for (auto i = heap_size; i < heap.size(); ++i)
      {
        combs[heap[i]].binary(combs[heap.front()]);
      }

      int comp_num = 0;
      for (auto i = heap_size; i < heap.size(); ++i)
      {
        if (combs[heap[i]].completed)
          ++comp_num;
        else if (comp_num > 0)
          heap[i - comp_num] = heap[i];
      }

      for (auto i = heap_size; i < (int)heap.size() - comp_num; i++)
      {
        push_heap(heap.begin(), heap.begin() + i + 1, comp_comb);
      }
      while (comp_num-- > 0)
        heap.pop_back();
      heap_size = heap.size();
    }
  }

  cout << "Res lists num: " << res_lists.size() << endl;

  vector<vector<int>> id_lists(n);
  for (auto i = 0; i < res_lists.size(); i++)
  {
    for (auto j = 0; j < res_lists[i].size(); j++)
      id_lists[res_lists[i][j]].push_back(i);
  }

  vector<int> results(n, -1);
  for (auto i = n - 1; i >= 0; i--)
  {
    if (id_lists[i].empty())
      continue;
    for (auto j = 0; j < id_lists[i].size(); j++)
    {
      res_lists[id_lists[i][j]].pop_back();
      for (auto k = 0; k < res_lists[id_lists[i][j]].size(); k++)
      {
        if (results[res_lists[id_lists[i][j]][k]] != i)
        {
          // cout << idmap[i].first << " " << idmap[res_lists[id_lists[i][j]][k]].first << endl;
          results[res_lists[id_lists[i][j]][k]] = i;
          int idd1 = idmap[i].first;
          int idd2 = idmap[res_lists[id_lists[i][j]][k]].first;
          result_pairs.emplace_back(idd1, idd2);

          ++result_num;
        }
      }
    }
  }
  ++c;
}

void OvlpJoin::overlapjoin(int overlap_threshold)
{
  srand(time(NULL));

  timeval starting, ending, s1, t1, s2, t2;
  timeval time1, time2, time3, time4;

  gettimeofday(&starting, NULL);

  c = overlap_threshold;           // get threshold
  n = records.size();              // get number of records
  buck.assign(n, make_pair(0, 0)); // for counting

  vector<pair<int, int>> eles;
  unordered_map<int, vector<int>> ele;

  for (int i = 0; i < records.size(); i++)
  {
    if (records[i].size() < c)
      continue;                                 // remove records with size smaller than c
    for (int j = 0; j < records[i].size(); j++) // build inverted index
      ele[records[i][j]].push_back(i);
  }

  for (auto it = ele.begin(); it != ele.end(); it++)
    eles.push_back(make_pair(it->first, it->second.size())); // build element frequency table

  // get global order: frequency increasing order
  // sort the elements
  sort(eles.begin(), eles.end(), [](const pair<int, int> &p1, const pair<int, int> &p2)
       { return p1.second < p2.second; });

  // container initialize
  dataset.resize(n);

  // sort elements by its global order: frequence increasing order
  // remove widow word
  // encode elements in decreasing order
  // so the dataset is the same as the record, only the element is encoded to 0~ total_eles-1. 
  // The encoding way is the less frequency of the element is, the large number it gets
  total_eles = eles.size();
  for (auto i = 0; i < int(eles.size()); ++i)
  {
    if (eles[i].second < 2)
      continue;
    for (auto j = ele[eles[i].first].begin(); j != ele[eles[i].first].end(); j++)
      dataset[*j].push_back(total_eles - i - 1);
  }

  gettimeofday(&time1, NULL);
  cout << "Initial Time: " << time1.tv_sec - starting.tv_sec + (time1.tv_usec - starting.tv_usec) / 1e6 << endl;

  // ****** cost model for prefix length selection ******
  // remove short records
  for (auto i = 0; i < n; i++)
    if (dataset[i].size() < c)
      dataset[i].clear();

  // create id mappings: from sorted to origin
  for (auto i = 0; i < n; i++)
    idmap.push_back(make_pair(i, dataset[i].size()));

  // sort records by length in decreasing order
  sort(idmap.begin(), idmap.end(), [](const pair<int, int> &a, const pair<int, int> &b)
       { return a.second > b.second; });
  sort(dataset.begin(), dataset.end(), [](const vector<unsigned short> &a, const vector<unsigned short> &b)
       { return a.size() > b.size(); });
  cout << " largest set: " << dataset.front().size() << " smallest set: " << dataset.back().size() << "It might be 0 cause some row in dataset, its length is smaller than c"<< endl;

  // build real inverted index
  ele_lists.resize(total_eles);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < dataset[i].size(); j++)
      ele_lists[dataset[i][j]].push_back(make_pair(i, j));

  gettimeofday(&time3, NULL);
  cout << "Transform Time: " << time3.tv_sec - time1.tv_sec + (time3.tv_usec - time1.tv_usec) / 1e6 << endl;

  // ****** cost model for boundary selection ******
  int nL = 0; //estimate();
  int nP = nL;
  cout << " All are treated as small sets: " << n - nP << endl;

  gettimeofday(&time4, NULL);
  // ****** conduct joining ******
  result_num = 0;
  candidate_num = 0;

  gettimeofday(&s1, NULL);
  gettimeofday(&t1, NULL);

  gettimeofday(&s2, NULL);
  small_case(nP, n);
  gettimeofday(&t2, NULL);

  gettimeofday(&ending, NULL);
  cout << "Join Time: " << ending.tv_sec - time4.tv_sec + (ending.tv_usec - time4.tv_usec) / 1e6 << endl;
  cout << "  small Time: " << t2.tv_sec - s2.tv_sec + (t2.tv_usec - s2.tv_usec) / 1e6 << endl;
  cout << "All Time: " << ending.tv_sec - starting.tv_sec + (ending.tv_usec - starting.tv_usec) / 1e6 << endl;
  cout << "Result Num: " << result_num << endl;
}


// find first set with size smaller or equal to nL
int OvlpJoin::divide(int nL)
{
  int l = 0, r = n;
  while (l < r)
  {
    int m = (l + r) >> 1;
    if (dataset[m].size() > nL)
      l = m + 1;
    else
      r = m;
  }
  return r;
}

bool comp_pair(const pair<int, int> &p1, const int val)
{
  return p1.first < val;
}

bool comp_int(const int a, const int b)
{
  return a > b;
}

bool comp_comb(const int a, const int b)
{
  combination &c1 = combs[a];
  combination &c2 = combs[b];
  for (int i = 0; i < c; i++)
  {
    if (dataset[c1.id][c1.curr[i]] > dataset[c2.id][c2.curr[i]])
      return false;
    else if (dataset[c1.id][c1.curr[i]] < dataset[c2.id][c2.curr[i]])
      return true;
  }
  return c1.id > c2.id;
}

bool OvlpJoin::is_equal(const combination &c1, const combination &c2)
{
  for (int i = 0; i < c; i++)
  {
    if (dataset[c1.id][c1.curr[i]] != dataset[c2.id][c2.curr[i]])
      return false;
  }
  return true;
}