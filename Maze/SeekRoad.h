class SeekRoad
{
public:
	int mv[4][2] = { 0,1,0,-1,-1,0,1,0 };
	SeekRoad();
	~SeekRoad();
	struct node {
		int r, c;
	};
	int seekroad(int maze[105][105], int maze_rows, int maze_cols, int *route, int crt_pos_x, int crt_pos_z, int save_pos_x, int save_pos_z);
};

