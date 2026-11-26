#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100;
const int GRID_SIZE = 20;
const int MIN_PER_GRID = 10;
const int DAY_START = 9 * 60;      // 09:00
const int DAY_END = 21 * 60 + 30;  // 21:30
const int DIST_WEIGHT = 3;

struct Spot {
    int id;
    string name;
    string cname;
    int x, y;
    int dur;                     // 停留時間（分鐘）
    int openH, closeH;           // 開放與關閉小時
    int mCrowd, aCrowd, eCrowd;  // 早 / 中 / 晚人潮
};

struct PlanStep {
    int day;
    int spotIdx;
    int arrMin, startMin, depMin;
    int dist, crowd, cost;
};

Spot spots[MAXN];
int spotCnt = 0;

int calcDist( int x1, int y1, int x2, int y2 ){
    return abs( x1 - x2 ) + abs( y1 - y2 );
}

int getCrowd( const Spot& s, int mins ){
    int hr = ( mins / 60 ) % 24;
    if( hr >= 6 && hr < 12 ) return s.mCrowd;
    if( hr >= 12 && hr < 18 ) return s.aCrowd;
    return s.eCrowd;
}

string fmtTime( int mins ){
    int hr = ( mins / 60 ) % 24;
    int mm = mins % 60;
    string hStr = ( hr < 10 ? "0" : "" ) + to_string( hr );
    string mStr = ( mm < 10 ? "0" : "" ) + to_string( mm );
    return hStr + ":" + mStr;
}

void initDefaultSpots(){
    spotCnt = 0;
    spots[spotCnt++] = { 0, "Miyahara", "宮原眼科", 9, 11, 60, 10, 21, 30, 85, 65 };
    spots[spotCnt++] = { 3, "Yizhong_Street", "一中商圈", 10, 10, 120, 11, 23, 20, 75, 95 };
    spots[spotCnt++] = { 6, "Shenji_New_Village", "審計新村", 7, 11, 90, 10, 21, 25, 85, 70 };
    spots[spotCnt++] = { 7, "NTMoFA", "國立臺灣美術館", 7, 12, 120, 9, 18, 45, 70, 15 };
    spots[spotCnt++] = { 16, "Fengchia_Night_Market", "逢甲觀光夜市", 6, 8, 120, 16, 25, 5, 25, 95 };
    spots[spotCnt++] = { 17, "National_Taichung_Theater", "臺中國家歌劇院", 6, 9, 120, 11, 21, 25, 65, 50 };
    spots[spotCnt++] = { 22, "Rainbow_Village", "彩虹村", 5, 11, 60, 9, 17, 75, 65, 10 };
    spots[spotCnt++] = { 35, "Gaomei_Wetlands", "高美濕地", 1, 5, 90, 0, 24, 20, 85, 40 };
    spots[spotCnt++] = { 41, "Houfeng_Bikeway", "后豐鐵馬道", 9, 3, 150, 6, 19, 65, 60, 10 };
}

int findSpotIdxById( int id ){
    for( int i = 0; i < spotCnt; i++ ){
        if( spots[i].id == id ) return i;
    }
    return -1;
}

bool tryVisit( int cx, int cy, int cMin, int idx, int& arr, int& st, int& dep, int& dist, int& crowd, int& cost ){
    const Spot& s = spots[idx];
    dist = calcDist( cx, cy, s.x, s.y );
    arr = cMin + dist * MIN_PER_GRID;

    int openMin = s.openH * 60;
    int closeMin = s.closeH * 60;
    if( arr >= closeMin ) return false;

    st = max( arr, openMin );
    dep = st + s.dur;
    if( dep > closeMin || dep > DAY_END ) return false;

    crowd = getCrowd( s, st );
    cost = crowd + dist * DIST_WEIGHT;
    return true;
}

void runGreedy( const vector<int>& wantIds, int days, int sx, int sy ){
    auto t0 = chrono::high_resolution_clock::now();

    vector<int> candIdx;
    for( int id : wantIds ){
        int idx = findSpotIdxById( id );
        if( idx != -1 ) candIdx.push_back( idx );
    }

    vector<bool> used( candIdx.size(), false );
    vector<PlanStep> ans;
    int totCost = 0, totDist = 0, totCrowd = 0;

    for( int d = 1; d <= days; d++ ){
        int cx = sx, cy = sy, cMin = DAY_START;

        while( true ){
            int bestK = -1, bestCost = INT_MAX;
            int bArr = 0, bSt = 0, bDep = 0, bDist = 0, bCrowd = 0;

            for( int k = 0; k < (int)candIdx.size(); k++ ){
                if( used[k] ) continue;
                int arr, st, dep, dist, crowd, cost;
                if( tryVisit( cx, cy, cMin, candIdx[k], arr, st, dep, dist, crowd, cost ) ){
                    if( cost < bestCost ){
                        bestCost = cost;
                        bestK = k;
                        bArr = arr; bSt = st; bDep = dep;
                        bDist = dist; bCrowd = crowd;
                    }
                }
            }

            if( bestK == -1 ) break;

            used[bestK] = true;
            int sIdx = candIdx[bestK];
            ans.push_back( { d, sIdx, bArr, bSt, bDep, bDist, bCrowd, bestCost } );

            totCost += bestCost;
            totDist += bDist;
            totCrowd += bCrowd;

            cx = spots[sIdx].x;
            cy = spots[sIdx].y;
            cMin = bDep;
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>( t1 - t0 ).count();

    cout << "=== Greedy 行程規劃結果 ===\n";
    for( const auto& step : ans ){
        const Spot& s = spots[step.spotIdx];
        cout << "Day " << step.day << " | "
             << fmtTime( step.startMin ) << "-" << fmtTime( step.depMin ) << " | "
             << s.cname << " (ID:" << s.id
             << ", 距離:" << step.dist
             << ", 人潮:" << step.crowd
             << ", 成本:" << step.cost << ")\n";
    }
    cout << "----------------------------\n";
    cout << "總成本: " << totCost << " | 總距離: " << totDist << " | 總人潮: " << totCrowd << "\n";
    cout << "耗時: " << fixed << setprecision(3) << ms << " ms\n";
}

int main(){
    initDefaultSpots();

    vector<int> wantIds = { 0, 3, 6, 7, 16, 17, 22, 35, 41 };
    int days = 2;
    int startX = 9, startY = 11;

    runGreedy( wantIds, days, startX, startY );
    return 0;
}
