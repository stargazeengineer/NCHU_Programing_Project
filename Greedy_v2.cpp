#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100;
const double MIN_PER_KM = 4.0;
const int DAY_START = 9 * 60;
const int DAY_END = 21 * 60 + 30;
const double DIST_WEIGHT = 3.0;
const double GRID_TO_KM = 2.5;
const int BENCHMARK_REPEAT = 1000;

struct Spot {
    int id;
    string name;
    string cname;
    int x, y;
    int dur;
    int openH, closeH;
    int mCrowd, aCrowd, eCrowd;
};

struct PlanStep {
    int day;
    int spotIdx;
    int arrMin, startMin, depMin;
    double distKm;
    int crowd;
    double cost;
};

struct UserConfig {
    int days = 2;
    int startX = 9;
    int startY = 11;
    double maxBudget = -1.0;
    vector<int> wantIds;
};

struct PlanResult {
    vector<PlanStep> ans;
    double totCost = 0.0;
    double totDistKm = 0.0;
    int totCrowd = 0;
    int candidatesChecked = 0;
    int candidatesRejected = 0;
};

Spot spots[MAXN];
int spotCnt = 0;

double distKmGraph[MAXN][MAXN];
bool hasExplicitEdge[MAXN][MAXN];

double calcApproxKm(int x1, int y1, int x2, int y2) {
    return (abs(x1 - x2) + abs(y1 - y2)) * GRID_TO_KM;
}

void setSpotDistKm(int fromIdx, int toIdx, double km) {
    if (fromIdx < 0 || fromIdx >= spotCnt ||
        toIdx < 0 || toIdx >= spotCnt) return;

    if (fromIdx == toIdx) {
        distKmGraph[fromIdx][toIdx] = 0.0;
        hasExplicitEdge[fromIdx][toIdx] = true;
        return;
    }

    distKmGraph[fromIdx][toIdx] = max(0.0, km);
    hasExplicitEdge[fromIdx][toIdx] = true;
}

void setSpotDistSymmetricKm(int idxA, int idxB, double km) {
    setSpotDistKm(idxA, idxB, km);
    setSpotDistKm(idxB, idxA, km);
}

void initDefaultSpots() {
    spotCnt = 0;

    spots[spotCnt++] = {0, "Miyahara", "Miyahara", 9, 11, 60, 10, 21, 30, 85, 65};
    spots[spotCnt++] = {3, "Yizhong_Street", "Yizhong_Street", 10, 10, 120, 11, 23, 20, 75, 95};
    spots[spotCnt++] = {6, "Shenji_New_Village", "Shenji_New_Village", 7, 11, 90, 10, 21, 25, 85, 70};
    spots[spotCnt++] = {7, "NTMoFA", "NTMoFA", 7, 12, 120, 9, 18, 45, 70, 15};
    spots[spotCnt++] = {16, "Fengchia_Night_Market", "Fengchia_Night_Market", 6, 8, 120, 16, 25, 5, 25, 95};
    spots[spotCnt++] = {17, "National_Taichung_Theater", "National_Taichung_Theater", 6, 9, 120, 11, 21, 25, 65, 50};
    spots[spotCnt++] = {22, "Rainbow_Village", "Rainbow_Village", 5, 11, 60, 9, 17, 75, 65, 10};
    spots[spotCnt++] = {35, "Gaomei_Wetlands", "Gaomei_Wetlands", 1, 5, 90, 0, 24, 20, 85, 40};
    spots[spotCnt++] = {41, "Houfeng_Bikeway", "Houfeng_Bikeway", 9, 3, 150, 6, 19, 65, 60, 10};
    spots[spotCnt++] = {48, "Taichung_Park", "Taichung_Park", 9, 10, 90, 6, 22, 40, 65, 55};
    spots[spotCnt++] = {52, "Calligraphy_Greenway", "Calligraphy_Greenway", 8, 11, 90, 0, 24, 30, 80, 60};
    spots[spotCnt++] = {55, "Second_Market", "Second_Market", 9, 11, 90, 7, 20, 55, 70, 25};
    spots[spotCnt++] = {61, "Lihpao_Resort", "Lihpao_Resort", 8, 2, 180, 9, 21, 35, 80, 50};
    spots[spotCnt++] = {64, "Dajia_Jenn_Lan_Temple", "Dajia_Jenn_Lan_Temple", 3, 3, 90, 5, 22, 45, 75, 60};
    spots[spotCnt++] = {68, "Xinshe_Castle", "Xinshe_Castle", 14, 7, 120, 9, 17, 30, 75, 15};
    spots[spotCnt++] = {72, "Dongfeng_Green_Way", "Dongfeng_Green_Way", 12, 5, 120, 6, 19, 55, 65, 15};
    spots[spotCnt++] = {77, "Zhongshe_Flower_Market", "Zhongshe_Flower_Market", 8, 4, 90, 8, 18, 35, 70, 20};
    spots[spotCnt++] = {83, "Lu_Tung_Night_Market", "Lu_Tung_Night_Market", 4, 12, 120, 16, 24, 5, 20, 85};
    spots[spotCnt++] = {88, "Taichung_Station", "Taichung_Station", 10, 11, 45, 0, 24, 55, 70, 65};
    spots[spotCnt++] = {91, "Chung_Yo_Dept_Store", "Chung_Yo_Dept_Store", 9, 9, 120, 11, 22, 20, 65, 80};
    spots[spotCnt++] = {95, "Park_Lane_By_CMP", "Park_Lane_By_CMP", 8, 11, 90, 10, 22, 30, 80, 75};
    spots[spotCnt++] = {99, "Dihua_Street_Taichung", "Dihua_Street_Taichung", 5, 7, 90, 9, 21, 35, 65, 55};
    spots[spotCnt++] = {104, "Wufeng_Lin_Family_Mansion", "Wufeng_Lin_Family_Mansion", 11, 15, 120, 9, 17, 45, 65, 15};
    spots[spotCnt++] = {108, "Asia_Museum_Of_Modern_Art", "Asia_Museum_Of_Modern_Art", 11, 16, 120, 9, 17, 35, 60, 10};
    spots[spotCnt++] = {112, "Wuqi_Fishing_Port", "Wuqi_Fishing_Port", 2, 6, 120, 8, 21, 25, 75, 60};
    spots[spotCnt++] = {116, "Qingshui_Cliff_Park", "Qingshui_Cliff_Park", 3, 5, 90, 0, 24, 30, 65, 25};
    spots[spotCnt++] = {120, "Dacun_Orchard_Trail", "Dacun_Orchard_Trail", 13, 6, 120, 6, 18, 50, 55, 15};
    spots[spotCnt++] = {125, "Guguan_Hot_Spring", "Guguan_Hot_Spring", 18, 4, 180, 7, 22, 30, 55, 45};
}

void initSpotDistGraph() {
    for (int i = 0; i < spotCnt; i++) {
        for (int j = 0; j < spotCnt; j++) {
            hasExplicitEdge[i][j] = false;

            if (i == j)
                distKmGraph[i][j] = 0.0;
            else
                distKmGraph[i][j] =
                    calcApproxKm(spots[i].x, spots[i].y,
                                 spots[j].x, spots[j].y);
        }
    }

    setSpotDistKm(0, 1, 1.4);
    setSpotDistKm(1, 0, 1.5);

    setSpotDistKm(0, 2, 2.5);
    setSpotDistKm(2, 0, 2.6);

    setSpotDistKm(2, 3, 1.2);
    setSpotDistKm(3, 2, 1.2);

    setSpotDistKm(4, 5, 2.0);
    setSpotDistKm(5, 4, 2.2);

    setSpotDistKm(0, 9, 1.1);
    setSpotDistKm(9, 0, 1.2);

    setSpotDistKm(0, 11, 0.8);
    setSpotDistKm(11, 0, 0.9);

    setSpotDistKm(2, 10, 1.3);
    setSpotDistKm(10, 2, 1.4);

    setSpotDistKm(8, 12, 3.5);
    setSpotDistKm(12, 8, 3.8);

    setSpotDistKm(8, 15, 7.2);
    setSpotDistKm(15, 8, 7.5);

    setSpotDistKm(0, 18, 0.7);
    setSpotDistKm(18, 0, 0.8);

    setSpotDistKm(1, 19, 1.2);
    setSpotDistKm(19, 1, 1.3);

    setSpotDistKm(2, 20, 1.0);
    setSpotDistKm(20, 2, 1.1);

    setSpotDistKm(4, 21, 2.4);
    setSpotDistKm(21, 4, 2.6);

    setSpotDistKm(22, 23, 2.2);
    setSpotDistKm(23, 22, 2.3);

    setSpotDistKm(7, 24, 18.5);
    setSpotDistKm(24, 7, 19.4);

    setSpotDistKm(24, 25, 6.5);
    setSpotDistKm(25, 24, 7.0);

    setSpotDistKm(1, 4, 6.8);
    setSpotDistKm(4, 1, 7.6);

    setSpotDistKm(5, 6, 7.5);
    setSpotDistKm(6, 5, 8.4);

    setSpotDistKm(2, 6, 6.2);
    setSpotDistKm(6, 2, 6.5);

    setSpotDistKm(4, 7, 21.0);
    setSpotDistKm(7, 4, 22.5);

    setSpotDistKm(0, 8, 26.5);
    setSpotDistKm(8, 0, 27.8);

    setSpotDistKm(7, 8, 24.0);
    setSpotDistKm(8, 7, 25.2);

    setSpotDistKm(1, 14, 18.5);
    setSpotDistKm(14, 1, 19.8);

    setSpotDistKm(14, 15, 9.5);
    setSpotDistKm(15, 14, 10.2);

    setSpotDistKm(8, 16, 5.4);
    setSpotDistKm(16, 8, 5.9);

    setSpotDistKm(7, 13, 14.5);
    setSpotDistKm(13, 7, 15.2);

    setSpotDistKm(3, 17, 11.0);
    setSpotDistKm(17, 3, 12.1);

    setSpotDistKm(18, 22, 13.5);
    setSpotDistKm(22, 18, 14.4);

    setSpotDistKm(14, 26, 6.8);
    setSpotDistKm(26, 14, 7.3);

    setSpotDistKm(15, 27, 29.0);
    setSpotDistKm(27, 15, 31.2);

    for (int i = 0; i < spotCnt; i++) {
        for (int j = 0; j < spotCnt; j++) {
            if (i != j && !hasExplicitEdge[i][j]) {
                distKmGraph[i][j] =
                    calcApproxKm(spots[i].x, spots[i].y,
                                 spots[j].x, spots[j].y);
            }
        }
    }
}

int findSpotIdxById(int id) {
    for (int i = 0; i < spotCnt; i++) {
        if (spots[i].id == id) return i;
    }
    return -1;
}

int findSpotIdxByPos(int x, int y) {
    for (int i = 0; i < spotCnt; i++) {
        if (spots[i].x == x && spots[i].y == y) return i;
    }
    return -1;
}

double calcDist(int x1, int y1, int x2, int y2) {
    if (x1 == x2 && y1 == y2) return 0.0;

    int fromIdx = findSpotIdxByPos(x1, y1);
    int toIdx = findSpotIdxByPos(x2, y2);

    if (fromIdx != -1 && toIdx != -1)
        return distKmGraph[fromIdx][toIdx];

    if (toIdx != -1) {
        double best = calcApproxKm(x1, y1, x2, y2);

        for (int i = 0; i < spotCnt; i++) {
            if (spots[i].x == x1 && spots[i].y == y1) {
                best = min(best, distKmGraph[i][toIdx]);
            }
        }

        return best;
    }

    return calcApproxKm(x1, y1, x2, y2);
}

int getCrowd(const Spot& s, int mins) {
    int hr = (mins / 60) % 24;

    if (hr >= 6 && hr < 12) return s.mCrowd;
    if (hr >= 12 && hr < 18) return s.aCrowd;

    return s.eCrowd;
}

string fmtTime(int mins) {
    int hr = (mins / 60) % 24;
    int mm = mins % 60;

    string hStr = (hr < 10 ? "0" : "") + to_string(hr);
    string mStr = (mm < 10 ? "0" : "") + to_string(mm);

    return hStr + ":" + mStr;
}

bool tryVisit(
    int cx, int cy, int cMin, int idx,
    int& arr, int& st, int& dep,
    double& distKm, int& crowd, double& cost
) {
    const Spot& s = spots[idx];

    distKm = calcDist(cx, cy, s.x, s.y);
    arr = cMin + static_cast<int>(round(distKm * MIN_PER_KM));

    int openMin = s.openH * 60;
    int closeMin = s.closeH * 60;

    if (arr >= closeMin) return false;

    st = max(arr, openMin);
    dep = st + s.dur;

    if (dep > closeMin || dep > DAY_END) return false;

    crowd = getCrowd(s, st);
    cost = crowd + distKm * DIST_WEIGHT;

    return true;
}

void printAvailableSpots() {
    cout << "=== Available Spots ===\n";

    for (int i = 0; i < spotCnt; i++) {
        cout << "ID: " << setw(3) << spots[i].id
             << " | Name: " << spots[i].cname
             << " | Pos: (" << spots[i].x << "," << spots[i].y << ")"
             << " | Dur: " << spots[i].dur << "m"
             << " | Open: " << spots[i].openH
             << "-" << spots[i].closeH << "\n";
    }

    cout << "\n";
}

UserConfig configureFromUser() {
    UserConfig cfg;

    for (int i = 0; i < spotCnt; i++) {
        cfg.wantIds.push_back(spots[i].id);
    }

    printAvailableSpots();

    cout << "Use default settings? (1 = custom, 0 = default): ";

    int mode = 0;

    if (!(cin >> mode) || mode != 1) {
        cout << "Using default configuration "
             << "(days=2, start=(9,11), all spots, no budget cap).\n\n";
        return cfg;
    }

    cout << "Enter number of days (e.g. 1..7): ";

    int dInput;

    if (cin >> dInput && dInput > 0)
        cfg.days = dInput;

    cout << "Enter start coordinates X Y "
         << "(or -1 -1 to keep default 9 11): ";

    int sxInput, syInput;

    if (cin >> sxInput >> syInput) {
        if (sxInput >= 0 && syInput >= 0) {
            cfg.startX = sxInput;
            cfg.startY = syInput;
        }
    }

    cout << "Enter max total cost budget (<= 0 for unlimited): ";

    double budgetInput;

    if (cin >> budgetInput)
        cfg.maxBudget = budgetInput;

    cout << "How many spots do you want to visit? "
         << "(0 = all default spots): ";

    int cntInput;

    if (cin >> cntInput && cntInput > 0) {
        vector<int> picked;

        cout << "Enter " << cntInput
             << " spot IDs separated by space: ";

        for (int i = 0; i < cntInput; i++) {
            int id;

            if (cin >> id) {
                if (findSpotIdxById(id) != -1)
                    picked.push_back(id);
                else
                    cout << "Warning: Spot ID " << id
                         << " not found, skipped.\n";
            }
        }

        if (!picked.empty())
            cfg.wantIds = picked;
    }

    cout << "\n";

    return cfg;
}

PlanResult buildGreedyPlan(const UserConfig& cfg) {
    PlanResult result;

    vector<int> candIdx;

    for (int id : cfg.wantIds) {
        int idx = findSpotIdxById(id);

        if (idx != -1)
            candIdx.push_back(idx);
    }

    vector<bool> used(candIdx.size(), false);

    bool hasBudget = (cfg.maxBudget > 0.0);

    for (int d = 1; d <= cfg.days; d++) {
        int cx = cfg.startX;
        int cy = cfg.startY;
        int cMin = DAY_START;

        while (true) {
            int bestK = -1;
            double bestCost = 1e18;

            int bArr = 0;
            int bSt = 0;
            int bDep = 0;
            int bCrowd = 0;

            double bDistKm = 0.0;

            for (int k = 0; k < (int)candIdx.size(); k++) {
                if (used[k])
                    continue;

                result.candidatesChecked++;

                int arr, st, dep, crowd;
                double distKm, cost;

                if (!tryVisit(
                    cx, cy, cMin,
                    candIdx[k],
                    arr, st, dep,
                    distKm, crowd, cost
                )) {
                    result.candidatesRejected++;
                    continue;
                }

                if (hasBudget &&
                    result.totCost + cost > cfg.maxBudget + 1e-9) {
                    result.candidatesRejected++;
                    continue;
                }

                if (cost < bestCost) {
                    bestCost = cost;
                    bestK = k;

                    bArr = arr;
                    bSt = st;
                    bDep = dep;
                    bDistKm = distKm;
                    bCrowd = crowd;
                }
            }

            if (bestK == -1)
                break;

            used[bestK] = true;

            int sIdx = candIdx[bestK];

            result.ans.push_back({
                d,
                sIdx,
                bArr,
                bSt,
                bDep,
                bDistKm,
                bCrowd,
                bestCost
            });

            result.totCost += bestCost;
            result.totDistKm += bDistKm;
            result.totCrowd += bCrowd;

            cx = spots[sIdx].x;
            cy = spots[sIdx].y;
            cMin = bDep;
        }
    }

    return result;
}


void runGreedy(const UserConfig& cfg) {

    volatile double benchmarkSink = 0.0;

    auto t0 = chrono::steady_clock::now();

    for (int i = 0; i < BENCHMARK_REPEAT; i++) {
        PlanResult temp = buildGreedyPlan(cfg);

        // Consume results to ensure calculations are used.
        benchmarkSink = benchmarkSink + temp.totCost
                      + temp.totDistKm
                      + temp.ans.size();
    }

    auto t1 = chrono::steady_clock::now();

    double totalMs =
        chrono::duration<double, milli>(t1 - t0).count();

    double avgMs = totalMs / BENCHMARK_REPEAT;

    PlanResult result = buildGreedyPlan(cfg);

    cout << "\n=== Greedy Itinerary Result ===\n";

    for (const auto& step : result.ans) {
        const Spot& s = spots[step.spotIdx];

        cout << "Day " << step.day << " | "
             << fmtTime(step.startMin) << "-"
             << fmtTime(step.depMin) << " | "
             << s.cname
             << " (ID:" << s.id
             << ", dist:" << fixed << setprecision(1)
             << step.distKm << "km"
             << ", crowd:" << step.crowd
             << ", cost:" << fixed << setprecision(1)
             << step.cost << ")\n";
    }

    cout << "----------------------------\n";

    cout << "Spots Visited: "
         << result.ans.size() << " / "
         << cfg.wantIds.size()
         << " | Total Cost: "
         << fixed << setprecision(1)
         << result.totCost
         << " | Total Dist: "
         << result.totDistKm << "km"
         << " | Total Crowd: "
         << result.totCrowd << "\n";

    cout << "\n=== Performance Statistics ===\n";

    cout << "Candidates Checked: "
         << result.candidatesChecked
         << " | Candidates Rejected: "
         << result.candidatesRejected << "\n";

    cout << "Benchmark Repetitions: "
         << BENCHMARK_REPEAT << "\n";

    cout << "Total Elapsed: "
         << fixed << setprecision(6)
         << totalMs << " ms\n";

    cout << "Average Elapsed: "
         << fixed << setprecision(9)
         << avgMs << " ms\n";

    // Prevent unused-variable warnings.
    (void)benchmarkSink;
}

int main() {
    initDefaultSpots();
    initSpotDistGraph();

    UserConfig cfg = configureFromUser();

    runGreedy(cfg);

    return 0;
}
