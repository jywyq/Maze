#include "SeekRoad.h"
#include<queue>

SeekRoad::SeekRoad()
{
}


SeekRoad::~SeekRoad()
{
}
int SeekRoad::seekroad(int maze[105][105], int maze_rows, int maze_cols, int *route, int crt_c, int crt_r, int save_c, int save_r) {
	if (maze[save_r][save_c] == 1) {
		puts("目的地错误");
		return -1;
	}
	int size = 0;
	std::queue<node> Q;
	bool vis[105][105]; memset(vis, 0, sizeof vis);
	int pre[105][105]; memset(pre, -1, sizeof pre);
	Q.push(node{ save_r,save_c }); vis[save_r][save_c] = 1;
	while (!Q.empty()) {
		node p = Q.front(); Q.pop();
		for (int i = 0; i < 4; i++) {
			int nr = p.r + mv[i][0];
			int nc = p.c + mv[i][1];
			if (0 <= nr && nr < maze_rows && 0 <= nc && nc < maze_cols && maze[nr][nc] == 0 && !vis[nr][nc]) {
				vis[nr][nc] = 1;
				pre[nr][nc] = i & 1 ? (i - 1) : (i + 1);
				Q.push(node{ nr,nc });
			}
		}
	}
	if (!vis[crt_r][crt_c]) {
		puts("没有合法路径");
		return -1;
	}
	int r = crt_r, c = crt_c;
	while (r != save_r || c != save_c) {
		int i = pre[r][c];
		route[size++] = i;
		r += mv[i][0]; c += mv[i][1];
		printf("r:%d c:%d\n", r, c);
		//maze[r][c] = 8;
	}
	return size;
}
