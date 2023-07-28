#include "Combination2.h"

extern bool comp_int(const int a, const int b);

// compute next combination2
void combination2::next() {
    int i = c - 1;
    while (i >= 0 && curr[i] == N - c + i)
        --i;
    if (i < 0)
        completed = true;
    else {
        int temp = curr[i];
        for (int j = i; j <= c - 1; j++)
            curr[j] = temp + 1 + j - i;
    }
}

void combination2::print() const {
    cout << "combination2 from " << id << " : ";
    for (auto j = 0; j < c; j++)
        cout << dataset2[id][curr[j]] << " ";
    cout << " ----> ";
    for (auto j = 0; j < c; j++)
        cout << curr[j] << " ";
    cout << endl;
}

bool combination2::stepback(const int i) {
    if (i == 0)
        return true;
    curr[i - 1]++;
    if (curr[i - 1] + c - 1 - i + 1 >= N)
        return stepback(i - 1);
    for (int j = i; j < c; j++)
        curr[j] = curr[i - 1] + j - i + 1;
    return false;
}

void combination2::binary(const combination2 &value) {
    auto it = dataset2[id].begin() + curr[0];
    for (int i = 0; i < c; i++) {
        // find the first one not larger than the value
        it = lower_bound(it, dataset2[id].end(), dataset2[value.id][value.curr[i]], comp_int);
        // if get the end, we will increase the last one by 1 and set the rest as max
        if (it == dataset2[id].end()) {
            completed = stepback(i);
            return;
            // if we get the same value, we fill in it
        } else if (*it == dataset2[value.id][value.curr[i]]) {
            curr[i] = distance(dataset2[id].begin(), it);
            // if we get the smaller value, we set the rest as max
        } else {
            curr[i] = distance(dataset2[id].begin(), it);
            if (curr[i] + c - 1 - i >= N) {
                completed = stepback(i);
                return;
            }
            for (int j = i + 1; j < c; j++)
                curr[j] = curr[i] + j - i;
            return;
        }
    }
    return;
}

void combination2::binary(const combination1 &value) {
    auto it = dataset2[id].begin() + curr[0];
    for (int i = 0; i < c; i++) {
        // find the first one not larger than the value
        it = lower_bound(it, dataset2[id].end(), dataset1[value.id][value.curr[i]], comp_int);
        // if get the end, we will increase the last one by 1 and set the rest as max
        if (it == dataset2[id].end()) {
            completed = stepback(i);
            return;
            // if we get the same value, we fill in it
        } else if (*it == dataset1[value.id][value.curr[i]]) {
            curr[i] = distance(dataset2[id].begin(), it);
            // if we get the smaller value, we set the rest as max
        } else {
            curr[i] = distance(dataset2[id].begin(), it);
            if (curr[i] + c - 1 - i >= N) {
                completed = stepback(i);
                return;
            }
            for (int j = i + 1; j < c; j++)
                curr[j] = curr[i] + j - i;
            return;
        }
    }
    return;
}
