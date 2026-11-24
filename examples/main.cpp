#include <bits/stdc++.h>
#include "GraphVisualizer.h"
using namespace std;
typedef long long ll;
const int N = 2e6+5;
const int Q = 2e6+5;
const int M = 1e6+5;
const ll inf = 1ll<<60;

ll re();

int n,m,s,t;

int head[N];
int to[N],nxt[N];
ll limit[N];
int dist[N];
int ent = 2;
int cur[N];
int que[N];
int gap[N];
ll excess[N];//盈余量

int backet[N];
int nxtActive[N];
int highest = 0;

int ed = 0;

void addEdge(int u,int v,ll w){
    AddEdge(u,v,to_string(w),1);
    to[ent] = v;
    limit[ent] = w;
    nxt[ent] = head[u];
    head[u] = ent++;
}

void join(int i){
    // printf("join %lld\n",i);
    highest = max(highest , dist[i]);
    nxtActive[i] = backet[dist[i]];
    // printf("set nxtActive %d = %d\n",i,nxtActive[i]);
    backet[dist[i]] = i;
}

void upd(int i,int new_dist){
    gap[dist[i]]--;
    dist[i] = new_dist;
    gap[dist[i]]++;
    cur[i] = head[i];//重置
}

void push(int u,int v,ll val,int e){
    // printf("push u(%d)--%lld-->v(%d)\n",u,val,v);
    excess[u] -= val;
    excess[v] += val;
    limit[e] -= val;
    limit[e^1] += val; 
}

void bfs(){
    for(int i = 1;i <= n;i++){
        dist[i] = n+1;
        cur[i] = head[i];
        excess[i] = 0;
    }
    dist[t] = 0;
    int l = 1,r = 0;
    que[++r] = t;
    while(l <= r){
        int p = que[l++];
        HighLight(p);
        Sleep(300);
        for(int i = head[p];i;i=nxt[i]){
            int v = to[i];
            ll lim = limit[i^1];
            if(lim == 0 || dist[v] != n+1) continue;
            HighLightEdge(p,v);
            Sleep(300);
            dist[v] = dist[p] + 1;
            que[++r] = v;
        }
    }
    
}

void relabel(int i){
    int h = dist[i];
    if(gap[h] == 1){
        //没了，触发gap优化
        //那么从[h,n-1]都没了说明，然后呢，我们必然是要回到源点对吧，那么走回去肯定是要加好几回label不如直接全设为n+1
        for(int j = 1;j <= n;j++){
            if(dist[j] >= h && dist[j] < n){
                upd(j,n+1);
            }
        }
    }else{
        //否则直接找可以到的最小dist
        int mindist = (n<<1)-1;
        for(int e = head[i];e;e=nxt[e]){
            int v = to[e];
            if(i != v && limit[e] > 0){
                mindist = min(mindist , dist[v]);
            }
        }
        upd(i,mindist+1);
    }
}

void release(int i){
    int e = cur[i];
    HighLight(i);
    while(excess[i] > 0){
        cur[i] = e;
        if(e){
            int v = to[e];
            HighLightEdge(i,v);
            ll lim = limit[e] ;
            if(dist[v] == dist[i] - 1 && lim > 0){
                if(v != s && v != t && excess[v] == 0){
                    //不是开始或结尾，并且之前没激活
                    join(v);
                }
                push(i,v,min(lim,excess[i]),e);
            }
            e = nxt[e];
        }else{
            relabel(i);
            e = cur[i];
        }
    }
}

ll hlpp(){
    bfs();
    if(dist[s] == n+1)return 0;
    dist[s] = n;
    for(int i = head[s];i;i=nxt[i]){
        int v = to[i];
        if (dist[v] < n){
            push(s,v,limit[i],i);
        }
    }
    gap[0] = 0;
    highest = -1;
    for(int i = 1;i <= n<<1;i++){
        gap[i] = backet[i] = 0;
    }
    for(int i = 1;i <= n;i++){
        gap[dist[i]]++;
    }
    for(int i = 1;i <= n;i++){
        if(i != s && i != t && excess[i] > 0){
            join(i);
        }
    }
    //初始化我们让s节点去传递
    while(highest >= 0){
        if(backet[highest] == 0){
            highest--;
        }
        else{
            // printf("get %lld\n",backet[highest]);
            int now = backet[highest];
            backet[highest] = nxtActive[backet[highest]];
            release(now);
            
        }
    }
    return excess[t];
}


int main(){
    freopen("file.in","r",stdin);
    freopen("file.out","w",stdout);
    InitGraph();
	if (!IsGraphConnected()) {
		cerr << "GraphVisualizer connection failed!" << endl;
		return 1;
	}
    n = re(),m = re(),s = re(),t = re();
    for(int i = 1;i <= m;i++){
        int u = re(),v = re(),w = re();
        addEdge(u,v,w);
        addEdge(v,u,0);
    }
    printf("%lld\n",hlpp());
    return 0;
}

ll re(){
    ll x = 0,t =1;
    char ch = getchar();
    while(ch < '0' || ch > '9'){
        if(ch=='-')t=-t;
        ch = getchar();
    }
    while('0' <= ch && ch <= '9'){
        x = (x<<3) + (x<<1) + ch-48;
        ch = getchar();
    }
    return x*t;
}