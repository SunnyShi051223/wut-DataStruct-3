#include "StdAfx.h"
#include "GameLogic.h"
#include "global.h"

CGameLogic::CGameLogic(void)
{
	m_nVexNum = 0;     //数组中实际元素的个数
	m_nCorner = 0;     //路径中的拐点数
}


CGameLogic::~CGameLogic(void)
{
}

//初始化游戏地图
void CGameLogic::InitMap(CGraph &graph)
{
	//游戏地图初始化，固定的值
	//int anTemp[4][4] = { 2, 0, 1, 3, 2, 2, 1, 3, 2, 1, 0, 0, 1, 3, 0, 3 };
	//随机生成地图
	int anTemp[MAX_VERTEX_NUM];
	//多少花色
	for (int i = 0; i < MAX_PIC_NUM; i++)
	{
		//重复数
		for (int j = 0; j < REPEAT_NUM; j++)
		{
			anTemp[i * REPEAT_NUM + j] = i;
		}
	}
	//设置种子
	srand((int)time(NULL));

	//随机任意交换两个数字
	for (int i = 0; i < 300; i++)
	{
		//随机得到两个坐标
		int nIndex1 = rand() % MAX_VERTEX_NUM;
		int nIndex2 = rand() % MAX_VERTEX_NUM;

		//交换两个数值
		int nTemp = anTemp[nIndex1];
		anTemp[nIndex1] = anTemp[nIndex2];
		anTemp[nIndex2] = nTemp;
	}

	//初始化顶点
	for (int i = 0; i < MAX_VERTEX_NUM; i++)
	{
		graph.AddVertex(anTemp[i]);
	}

	//更新弧信息
	for (int i = 0; i < MAX_ROW; i++)
	{
		for (int j = 0; j < MAX_COL; j++)
		{
			UpdateArc(graph, i, j);
		}
	}
}

//连接判断函数
bool CGameLogic::IsLink(CGraph &graph, Vertex v1, Vertex v2)
{
	//获得顶点索引号
	int nV1Index = v1.row * MAX_COL + v1.col;
	int nV2Index = v2.row * MAX_COL + v2.col;

	PushVertex(nV1Index);  //压入第一个点

	//搜寻两点之间的连通路径
	if (SearchPath(graph, nV1Index, nV2Index) == true)
	{
		return true;
	}

	PopVertex();

	/*修改判断外圈可连接*/

	if (v1.row == v2.row) {
		if (v1.row == 0 || v1.row == MAX_ROW - 1) {
			return true;
		}
	}

	if (v1.col == v2.col) {
		if (v1.col == 0 || v1.col == MAX_COL - 1) {
			return true;
		}
	}
	return false;
}


//添加一个路径顶点
void CGameLogic::PushVertex(int nV)
{
	//将顶点压栈
	m_anPath[m_nVexNum] = nV;
	m_nVexNum++;

	//判断是否形成新的拐点
	if (IsCornor())
	{
		m_nCorner++;
	}
}

//取出一个路径顶点
void CGameLogic::PopVertex()
{
	//判断是否形成拐点，如果存在拐点，在取出之前减少拐点数
	if (IsCornor())
	{
		m_nCorner--;
	}
	m_nVexNum--;
}

//得到路径，返回的是顶点数
int CGameLogic::GetVexPath(Vertex avPath[MAX_VERTEX_NUM])
{
	Vertex point;
	for (int i = 0; i < m_nVexNum; i++)
		for (point.col = 0; point.col < MAX_COL; point.col++)
		{
			for (point.row = 0; point.row < MAX_ROW; point.row++)
			{
				if (point.row * MAX_COL + point.col == m_anPath[i])
				{
					avPath[i] = point;
				}
			}
		}
	return m_nVexNum;
}

//消子函数
void CGameLogic::Clear(CGraph &graph, Vertex v1, Vertex v2)
{
	//获得顶点索引号
	int nV1Index = v1.row * MAX_COL + v1.col;
	int nV2Index = v2.row * MAX_COL + v2.col;

	//更新顶点
	graph.UpdateVertex(nV1Index, BLANK);
	graph.UpdateVertex(nV2Index, BLANK);

	//更新边信息
	UpdateArc(graph, v1.row, v1.col);
	UpdateArc(graph, v2.row, v2.col);
}

//生成图边数组
void CGameLogic::UpdateArc(CGraph &graph, int nRow, int nCol)
{
	int nV1Index = nRow * MAX_COL + nCol;
	if (nCol > 0)   //左边相邻
	{
		int nV2Index = nV1Index - 1;
		int nV1Info = graph.GetVertex(nV1Index);
		int nV2Info = graph.GetVertex(nV2Index);

		//判断与左边相邻的是否有关系
		if (nV1Info == nV2Info || nV1Info == BLANK || nV2Info == BLANK)
		{
			graph.AddArc(nV1Index, nV2Index);
		}
	}
	if (nCol < MAX_COL - 1)   //右边相邻
	{
		int nV2Index = nV1Index + 1;
		int nV1Info = graph.GetVertex(nV1Index);
		int nV2Info = graph.GetVertex(nV2Index);

		//判断与右边相邻的是否有关系
		if (nV1Info == nV2Info || nV1Info == BLANK || nV2Info == BLANK)
		{
			graph.AddArc(nV1Index, nV2Index);
		}
	}
	if (nRow > 0)   //正上方相邻
	{
		int nV2Index = nV1Index - MAX_COL;
		int nV1Info = graph.GetVertex(nV1Index);
		int nV2Info = graph.GetVertex(nV2Index);

		//判断与正上方的是否有关系
		if (nV1Info == nV2Info || nV1Info == BLANK || nV2Info == BLANK)
		{
			graph.AddArc(nV1Index, nV2Index);
		}
	}
	if (nRow < MAX_ROW - 1)   //正下方相邻
	{
		int nV2Index = nV1Index + MAX_COL;
		int nV1Info = graph.GetVertex(nV1Index);
		int nV2Info = graph.GetVertex(nV2Index);

		//判断与正下方的是否有关系
		if (nV1Info == nV2Info || nV1Info == BLANK || nV2Info == BLANK)
		{
			graph.AddArc(nV1Index, nV2Index);
		}
	}
}

//使用深度优先搜索法搜寻一条有效连通路径
bool CGameLogic::SearchPath(CGraph &graph, int nV0, int nV1)
{
	//得到顶点数
	int nVexnum = graph.GetVexnum();

	//遍历图中nV0行，从0列到nVexnum列，值为true的点
	for (int nVi = 0; nVi < nVexnum; nVi++)
	{
		if (graph.GetArc(nV0, nVi) && !IsExsit(nVi))
		{
			//压入当前顶点，假设为路径的一个有效顶点
			PushVertex(nVi);
			//当拐点数大于2 时，直接放弃该顶点
			if (m_nCorner > 2)
			{
				PopVertex();          //取出压入的顶点，与PushWertex(nVi)对应
				continue;
			}
			//当前顶点不是nVi时，继续搜寻下一个相邻且连通的顶点
			if (nVi != nV1)
			{
				//当中间顶点不为空时，表示该条路径不通
				if (graph.GetVertex(nVi) != BLANK)
				{
					PopVertex();      //取出压入的顶点，与PushWertex(nVi)对应
					continue;
				}
				//如果nVi是一个已消除的点，则判断（nVi，nV1）是否连通
				if (SearchPath(graph, nVi, nV1))
				{
					//SearchPath(garph, nVi, nV1) == true,表示已经找到一条连通路径，则返回true
					return true;
				}
			}
			else
			{
				return true;
			}

			PopVertex();     //取出压入的顶点，与PushWertex(nVi)对应
		}
	}
	return false;
}

//判断顶点是否已在路径中存在
bool CGameLogic::IsExsit(int nVi)
{
	for (int i = 0; i < m_nVexNum; i++)
	{
		if (m_anPath[i] == nVi)
		{
			return true;
		}
	}
	return false;
}

//判断拐点的有效性
bool CGameLogic::IsCornor(void)
{
	if (m_nVexNum >= 3)
	{
		if ((m_anPath[m_nVexNum - 1] + m_anPath[m_nVexNum - 3]) / 2 != m_anPath[m_nVexNum - 2])
		{
			return true;
		}
	}
	return false;
}

//判断图中顶点是不是为空
bool CGameLogic::IsBlank(CGraph &graph)
{
	int nVexnum = graph.GetVexnum();
	for (int i = 0; i < nVexnum; i++)
	{
		if (graph.GetVertex(i) != BLANK)
		{
			return false;
		}
	}
	return true;
}

bool CGameLogic::SearchValidPath(CGraph& graph)
{
	//得到顶点数
	int nVexnum = graph.GetVexnum();
	for (int i = 0; i < nVexnum; i++)
	{
		//得到第一个非空顶点
		if (graph.GetVertex(i) == BLANK)
		{
			continue;
		}
		//遍历得到第二个同色顶点
		for (int j = 0; j < nVexnum; j++)
		{
			if (i != j)
			{
				//如果第i个点和第j个点同色
				if (graph.GetVertex(i) == graph.GetVertex(j))
				{
					//压入第一个点
					PushVertex(i);
					if (SearchPath(graph, i, j) == true)
					{
						return true;
					}
					//取出压入的顶点时，与PushVertex(i);对应
					PopVertex();
				}

			}
		}
	}
	return false;
}

//实现图结构的重排
void CGameLogic::ResetGraph(CGraph& graph)
{
	//随机交换顶点数组中两个顶点的值
	for (int i = 0; i < 200; i++)
	{
		//随机得到两个坐标
		int nIndex1 = rand() % MAX_VERTEX_NUM;
		int nIndex2 = rand() % MAX_VERTEX_NUM;

		//交换两个数值
		graph.ChangeVerex(nIndex1, nIndex2);
	}

	//更新弧信息
	for (int i = 0; i < MAX_ROW; i++)
	{
		for (int j = 0; j < MAX_COL; j++)
		{
			UpdateArc(graph, i, j);
		}
	}

}
