#include <iostream>

using namespace std;

struct Task {
    int id;
    double p;
};

double getMax(double a, double b) {
    return (a > b) ? a : b;
}

double getAbs(double a) {
    return (a < 0) ? -a : a;
}

class Scheduler {
private:
    Task** tasks;
    int task_count;
    int capacity;
    int total_added;

    void expand() {
        capacity *= 2;
        Task** new_tasks = new Task*[capacity];
        for (int i = 0; i < task_count; i++) new_tasks[i] = tasks[i];
        delete[] tasks;
        tasks = new_tasks;
    }
    void merge(Task** arr, int l, int m, int r, bool lpt) {
        int n1 = m - l + 1;
        int n2 = r - m;
        Task** L = new Task*[n1];
        Task** R = new Task*[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[l + i];
        for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            bool leftFirst = false;
            double diff = L[i]->p - R[j]->p;

            if (getAbs(diff) < 1e-9) {
                // Tie-breaker by ID
                leftFirst = lpt ? (L[i]->id > R[j]->id) : (L[i]->id < R[j]->id);
            } else {
                // Sort by processing time
                leftFirst = lpt ? (diff > 0) : (diff < 0);
            }

            if (leftFirst) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }

        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];

        delete[] L;
        delete[] R;
    }

    void mergeSort(Task** arr, int l, int r, bool lpt) {
        if (l < r) {
            int m = l + (r - l) / 2;
            mergeSort(arr, l, m, lpt);
            mergeSort(arr, m + 1, r, lpt);
            merge(arr, l, m, r, lpt);
        }
    }
    void sortTasks(Task** arr, int n) {
        for (int i = 1; i < n; i++) {
            Task* key = arr[i];
            int j = i - 1;
            while (j >= 0) {
                bool swap = false;
                double diff = arr[j]->p - key->p;



                    if (diff > 1e-9) swap = true;

                    else if (getAbs(diff) < 1e-9 && arr[j]->id > key->id) swap = true;


                if (swap) {
                    arr[j + 1] = arr[j];
                    j--;
                } else break;
            }
            arr[j + 1] = key;
        }
    }

public:
    Scheduler() {
        capacity = 10;
        task_count = 0;
        total_added = 0;
        tasks = new Task*[capacity];
    }

    ~Scheduler() {
        for (int i = 0; i < task_count; i++) delete tasks[i];
        delete[] tasks;
    }

    void add(int pos, double p) {
        if (task_count == capacity) expand();
        total_added++;
        Task* t = new Task{total_added, p};
        for (int i = task_count; i >= pos; i--) tasks[i] = tasks[i - 1];
        tasks[pos - 1] = t;
        task_count++;
    }

    void remove(int id) {
        int idx = -1;
        for (int i = 0; i < task_count; i++) if (tasks[i]->id == id) { idx = i; break; }
        if (idx != -1) {
            delete tasks[idx];
            for (int i = idx; i < task_count - 1; i++) tasks[i] = tasks[i + 1];
            task_count--;
        }
    }

    void printVal(double v) {
        if (getAbs(v - (long long)(v + 1e-11)) < 1e-9) cout << (long long)(v + 1e-11);
        else cout << v;
    }

    void runBL(int m, char mode) {
        Task** list = new Task*[task_count];
        for (int i = 0; i < task_count; i++) list[i] = tasks[i];
        if (mode == 'L') mergeSort(list, 0, task_count - 1, true);

        double* m_time = new double[m];
        for (int i = 0; i < m; i++) m_time[i] = 0;

        int** res_id = new int*[m];
        double** res_c = new double*[m];
        int* counts = new int[m];
        for(int i = 0; i < m; ++i) {
            res_id[i] = new int[task_count];
            res_c[i] = new double[task_count];
            counts[i] = 0;
        }

        double cmax = 0, sigmaC = 0;
        for (int i = 0; i < task_count; i++) {
            int target = 0;

            for (int j = 1; j < m; j++) {
                if (m_time[j] < m_time[target] - 1e-9) target = j;
            }
            m_time[target] += list[i]->p;
            res_id[target][counts[target]] = list[i]->id;
            res_c[target][counts[target]] = m_time[target];
            counts[target]++;
            if (m_time[target] > cmax) cmax = m_time[target];
            sigmaC += m_time[target];
        }

        cout << "Cmax: "; printVal(cmax);
        cout << "\nsigmaC: "; printVal(sigmaC); cout << "\n";
        for (int i = 0; i < m; i++) {
            cout << "M" << i + 1 << ":";
            for (int j = 0; j < counts[i]; j++) {
                cout << "( C" << res_id[i][j] << " = "; printVal(res_c[i][j]); cout << " )";
            }
            cout << "\n";
            delete[] res_id[i]; delete[] res_c[i];
        }
        delete[] res_id; delete[] res_c; delete[] counts; delete[] m_time; delete[] list;
    }

    void runS(int m) {
        Task** list = new Task*[task_count];
        for (int i = 0; i < task_count; i++) list[i] = tasks[i];
        mergeSort(list, 0, task_count - 1, true);

        Task*** groups = new Task**[m];
        int* g_counts = new int[m];
        for (int i = 0; i < m; i++) {
            groups[i] = new Task*[task_count];
            g_counts[i] = 0;
        }

        for (int i = 0; i < task_count; i++) groups[i % m][g_counts[i % m]++] = list[i];

        double cmax = 0, sigmaC = 0;
        for (int i = 0; i < m; i++) {
            sortTasks(groups[i], g_counts[i]);
            double curr = 0;
            for (int j = 0; j < g_counts[i]; j++) curr += groups[i][j]->p;
            if (curr > cmax) cmax = curr;
        }

        cout << "Cmax: "; printVal(cmax);
        cout << "\nsigmaC: ";
        for (int i = 0; i < m; i++) {
            double curr = 0;
            for (int j = 0; j < g_counts[i]; j++) {
                curr += groups[i][j]->p;
                sigmaC += curr;
            }
        }
        printVal(sigmaC); cout << "\n";

        for (int i = 0; i < m; i++) {
            cout << "M" << i + 1 << ":";
            double curr = 0;
            for (int j = 0; j < g_counts[i]; j++) {
                curr += groups[i][j]->p;
                cout << "( C" << groups[i][j]->id << " = "; printVal(curr); cout << " )";
            }
            cout << "\n";
            delete[] groups[i];
        }
        delete[] groups; delete[] g_counts; delete[] list;
    }

    void runM(int m) {
        double sum_p = 0, max_p = 0;
        for (int i = 0; i < task_count; i++) {
            sum_p += tasks[i]->p;
            if (tasks[i]->p > max_p) max_p = tasks[i]->p;
        }
        double cmax = getMax(max_p, sum_p / (double)m);


        double total_sigmaC = 0;
        double temp_t = 0;
        for (int i = 0; i < task_count; i++) {
            double p = tasks[i]->p;
            if (temp_t + p <= cmax + 1e-9) {
                temp_t += p;
                total_sigmaC += temp_t;
                if (cmax - temp_t < 1e-9) temp_t = 0;
            } else {
                total_sigmaC += cmax;
                temp_t = p - (cmax - temp_t);
            }
        }

        cout << "Cmax: "; printVal(cmax);
        cout << "\nsigmaC: "; printVal(total_sigmaC); cout << "\n";


        double cur_t = 0;
        int cur_m = 1;
        cout << "M1:";
        for (int i = 0; i < task_count; i++) {
            double p = tasks[i]->p;
            bool was_split = false;

            while (p > 1e-9) {
                if (cmax - cur_t < 1e-9) {
                    cur_m++;
                    cur_t = 0;
                    cout << "\nM" << cur_m << ":";
                }

                double space = cmax - cur_t;
                if (space >= p - 1e-9) {
                    cur_t += p;

                    if (was_split) {
                        cout << "( P" << tasks[i]->id << " = "; printVal(cur_t); cout << " )";
                    } else {
                        cout << "( C" << tasks[i]->id << " = "; printVal(cur_t); cout << " )";
                    }
                    p = 0;
                } else {

                    cout << "( C" << tasks[i]->id << " = "; printVal(cmax); cout << " )";
                    p -= space;
                    cur_t = cmax;
                    was_split = true;
                }
            }
        }
        while (cur_m < m) {
            cur_m++;
            cout << "\nM" << cur_m << ":";
        }
        cout << "\n";
    }

    double solveA(int m, int idx, double* loads) {
        if (idx == task_count) {
            double mx = 0;
            for (int i = 0; i < m; i++) if (loads[i] > mx) mx = loads[i];
            return mx;
        }
        double best = 1e18;
        for (int i = 0; i < m; i++) {
            loads[i] += tasks[idx]->p;
            double r = solveA(m, idx + 1, loads);
            if (r < best) best = r;
            loads[i] -= tasks[idx]->p;
            if (loads[i] < 1e-9) break;
        }
        return best;
    }
};

int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    Scheduler s;
    int n; if (!(cin >> n)) return 0;
    for (int i = 0; i < n; i++) { double p; cin >> p; s.add(i + 1, p); }
    char cmd;
    while (cin >> cmd) {
        if (cmd == '+') { int k; double p; cin >> k >> p; s.add(k, p); }
        else if (cmd == '-') { int id; cin >> id; s.remove(id); }
        else if (cmd == 'A') {
            int m; cin >> m;
            double* l = new double[m];
            for (int i = 0; i < m; i++) l[i] = 0;
            cout << "Copt: "; s.printVal(s.solveA(m, 0, l)); cout << "\n";
            delete[] l;
        } else {
            int m; cin >> m;
            if (cmd == 'M') s.runM(m);
            else if (cmd == 'S') s.runS(m);
            else s.runBL(m, cmd);
        }
    }
    return 0;
}