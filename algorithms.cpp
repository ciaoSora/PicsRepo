#include <cstdio> 	
#include <cstring>
#include <algorithm>
#include <climits>
#include <functional>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>

const int REPEAT_TIMES = 1000;

class Solver {
protected:
	int n, C;
	int *weight;
	void getInput(int n, int C, const int *weight) {
		this->n = n;
		this->C = C;
		if (this->weight != nullptr) {
			delete[] this->weight;
		}
		this->weight = new int[n];
		memcpy(this->weight, weight, sizeof(weight[0]) * n);
	}
public:
	Solver() {
		weight = nullptr;
	}
	virtual ~Solver() {
		delete[] weight;
		weight = nullptr;
	}
	virtual void init(int n, int C, const int *weight) = 0;
	virtual int solve() = 0;
};

class Solver_FF : public Solver {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
	}
	virtual int solve() {
		int *capacity = new int[n];
		int m = 0;

		for (int i = 0; i < n; ++i) {
			int j;
			for (j = 0; j < m; ++j) {
				if (weight[i] <= capacity[j]) {
					break;
				}
			}

			if (j < m) {
				capacity[j] -= weight[i];
			}
			else {
				capacity[m] = C - weight[i];
				++m;
			}
		}

		delete[] capacity;
		return m;
	}
};

class Solver_NF : public Solver {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
	}
	virtual int solve() {
		int *capacity = new int[n];
		int m = 1;
		capacity[0] = C - weight[0];
		int cur = 0;
		for (int i = 1; i < n; ++i) {
			int j = cur;
			do {
				if (weight[i] <= capacity[j]) {
					break;
				}
				j = (j + 1) % m;
			} while (j != cur);

			if (weight[i] <= capacity[j]) {
				cur = j;
				capacity[j] -= weight[i];
			}
			else {
				capacity[m] = C - weight[i];
				cur = m;
				++m;
			}
		}

		delete[] capacity;
		return m;
	}
};

class Solver_BF : public Solver {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
	}
	virtual int solve() {
		int *capacity = new int[n];
		int m = 0;

		for (int i = 0; i < n; ++i) {
			int min_data = INT_MAX, min_j = -1;
			for (int j = 0; j < m; ++j) {
				if (capacity[j] >= weight[i] && min_data > capacity[j]) {
					min_data = capacity[j];
					min_j = j;
				}
			}
			if (min_j == -1) {
				capacity[m] = C - weight[i];
				++m;
			}
			else {
				capacity[min_j] -= weight[i];
			}
		}
		return m;
	}
};

class Solver_WF : public Solver {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
	}
	virtual int solve() {
		int *capacity = new int[n];
		int m = 0;

		for (int i = 0; i < n; ++i) {
			int max_data = 0, max_j = -1;
			for (int j = 0; j < m; ++j) {
				if (max_data < capacity[j]) {
					max_data = capacity[j];
					max_j = j;
				}
			}
			if (max_data < weight[i]) {
				capacity[m] = C - weight[i];
				++m;
			}
			else {
				capacity[max_j] -= weight[i];
			}
		}
		return m;
	}
};

class Solver_ShuffledFF : public Solver_FF {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
		std::random_shuffle(this->weight, this->weight + n);
	}
};

class Solver_SortedRF : public Solver {
public:
	virtual void init(int n, int C, const int *weight) {
		getInput(n, C, weight);
		std::sort(this->weight, this->weight + n, std::greater<int>());
	}
	virtual int solve() {
		int *capacity = new int[n];
		int m = 0;

		for (int i = 0; i < n; ++i) {
			int cnt = 0;
			for (int j = 0; j < m; ++j) {
				if (weight[i] <= capacity[j]) {
					++cnt;
				}
			}
			if (cnt == 0) {
				capacity[m] = C - weight[i];
				++m;
				continue;
			}
			int x = rand() % cnt;
			cnt = 0;
			for (int j = 0; j < m; ++j) {
				if (weight[i] <= capacity[j] && cnt++ == x) {
					capacity[j] -= weight[i];
					break;
				}
			}
		}
		return m;
	}
};

int main() {
	Solver *solvers[6];
	solvers[0] = new Solver_FF();
	solvers[1] = new Solver_NF();
	solvers[2] = new Solver_BF();
	solvers[3] = new Solver_WF();
	solvers[4] = new Solver_ShuffledFF();
	solvers[5] = new Solver_SortedRF();

	timeval current_time;
	gettimeofday(&current_time, nullptr);
	srand(current_time.tv_sec * 1000000 + current_time.tv_usec);

	DIR *dir = opendir("Data");
	if (dir == nullptr) {
		printf("'Data' folder not found\n");
		return 0;
	}

	dirent *ent;
	FILE *fin;
	FILE *fres = fopen("result.txt", "w");
	FILE *ftime = fopen("time.txt", "w");
	char filename[40];
	while ((ent = readdir(dir)) != nullptr) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
			continue;
		}
		sprintf(filename, "Data/%s", ent->d_name);
		fin = fopen(filename, "r");
		int tidx = 0;
		for (int i = 0; ent->d_name[i]; ++i) {
			if (ent->d_name[i] == '.') {
				break;
			}
			if (ent->d_name[i] == '_') {
				filename[tidx++] = '\\';
				filename[tidx++] = '_';
			}
			else {
				filename[tidx++] = ent->d_name[i];
			}
		}
		filename[tidx] = '\0';
		fprintf(fres, "%s ", filename);
		fprintf(ftime, "%s ", filename);

		int n, C, *weight;
		fscanf(fin, "%d", &n);
		fscanf(fin, "%d", &C);
		weight = new int[n];
		for (int i = 0; i < n; ++i) {
			fscanf(fin, "%d", weight + i);
		}

		for (int i = 0; i < 6; ++i) {
			int tot_res = 0;
			timeval tv;
			gettimeofday(&tv, nullptr);
			long start_time = tv.tv_sec * 1000000 + tv.tv_usec;
			for (int j = 0; j < REPEAT_TIMES; ++j) {
				solvers[i]->init(n, C, weight);
				tot_res += solvers[i]->solve();
			}
			gettimeofday(&tv, nullptr);
			long end_time = tv.tv_sec * 1000000 + tv.tv_usec;

			fprintf(fres, "& %.2f ", (double)tot_res / REPEAT_TIMES);
			fprintf(ftime, "& %.2f ", (double)(end_time - start_time) / REPEAT_TIMES);
		}
		fprintf(fres, "\\\\\n");
		fprintf(ftime, "\\\\\n");

		delete[] weight;
	}

	for (int i = 0; i < 6; ++i) {
		delete solvers[i];
	}
	return 0;
}
