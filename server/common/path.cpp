#include "pch.h"
#include "path.h"
#include <string.h>
CPath::CPath()
{
	m_dir[0][0]= 0; m_dir[0][1]=-1;
	m_dir[1][0]= 0; m_dir[1][1]= 1;
	m_dir[2][0]= 1; m_dir[2][1]= 0;
	m_dir[3][0]=-1; m_dir[3][1]= 0;
	m_dir[4][0]= 1; m_dir[4][1]=-1;
	m_dir[5][0]=-1; m_dir[5][1]=-1;
	m_dir[6][0]= 1; m_dir[6][1]= 1;
	m_dir[7][0]=-1; m_dir[7][1]= 1;
	memset(m_arrOpenTable, 0, sizeof(m_arrOpenTable));
	memset(m_arrCheckedTable, 0, sizeof(m_arrCheckedTable));
	memset(m_mapElement, 0, sizeof(m_mapElement));
}

CPath::~CPath()
{
}
void CPath::Init(const uint8_t* attribute,uint8_t width,uint8_t height)
{
	m_Attr = attribute;
	m_width = width;
	m_height = height;
	m_OpenCount = 0;
	m_checkedNum = 0;
}
bool CPath::FindPath(const SPoint& src, const SPoint& tgt, std::vector<SPoint>& path)
{
	if(!CheckPoint(src) || !CheckPoint(tgt))
		return false;
	if (src.x == tgt.x && src.y == tgt.y)
	{
		path.emplace_back(src);
		return true;
	}
	SElement* ele = FindPath(src.x, src.y, tgt.x, tgt.y);
	if (nullptr == ele)
		return false;
	path.resize(ele->g +size_t( 1));
	while (ele)
	{
		path[ele->g].x = ele->x;
		path[ele->g].y = ele->y;
		ele = ele->father;
	}
	return true;
}

CPath::SElement* CPath::FindPath(uint8_t sx, uint8_t sy, uint8_t tx, uint8_t ty)
{
	uint8_t dir_x			;	// 待估坐标x
	uint8_t dir_y			;	// 待估坐标y
	uint16_t g				;	// 路径代价
	uint16_t total			;	// 总代价
	SElement* dir_node		;	// 待估节点
	SElement* current =  &(m_mapElement[sx][sy]);
	current->g = 0;
	current->total = 0;
	current->father = nullptr;
	current->x = sx;
	current->y = sy;
	current->index = m_checkedNum;
	m_arrCheckedTable[m_checkedNum++] = current;
	m_arrOpenTable[m_OpenCount++] = current;
	while(m_OpenCount > 0) 
	{
		current = m_arrOpenTable[--m_OpenCount];
		current->stat = enmClosed;
		if (current->x == tx && current->y == ty)
			return current;
		for (uint8_t i = 0; i < 8; ++i)
		{
			dir_x = current->x + m_dir[i][0];
			dir_y = current->y + m_dir[i][1];
			if (!CheckPoint(dir_x, dir_y) || !m_Attr[(uint16_t(dir_y)<<8)|dir_x])//是否越界和是否为阻挡点
				continue;
			//可以测试下对角线算法 g = current->g+2 x1 = |dir_x-tx| y1=|dir_y - ty| total = g + x1 + y1 + (x1 > y1 ? x1 : y1)
			g = current->g + 1; 
			total = g + (dir_x > tx ? dir_x - tx : tx - dir_x) + (dir_y > ty ? dir_y - ty : ty - dir_y);
			dir_node = &(m_mapElement[dir_x][dir_y]);
			if (dir_node->index >= m_checkedNum ||
				dir_node != m_arrCheckedTable[dir_node->index])//未访问过的节点
			{
				dir_node->g = g;
				dir_node->total = total;
				dir_node->father = current;
				dir_node->stat = enmOpen;
				dir_node->x = dir_x;
				dir_node->y = dir_y;
				dir_node->index = m_checkedNum;
				m_arrCheckedTable[m_checkedNum++] = dir_node;
				InsertOpen(dir_node);
				
			}
			else if (dir_node->total > total)//已经访问过的节点
			{
				dir_node->g = g;
				dir_node->total = total;
				dir_node->father = current;
				if (enmClosed == dir_node->stat)
				{
					dir_node->stat = enmOpen;
					InsertOpen(dir_node);
				}
			}
		}
	}
	return nullptr;
}
void CPath::InsertOpen(SElement* pEle)
{
	//使用插入排序，先使用二分查找到插入位置，按照从大到小排序
	int32_t nLeft = 0;
	int32_t nRight = m_OpenCount-1;
	int32_t nMid = 0;
	while(nLeft <= nRight)
	{
		nMid = (nLeft+nRight) >> 1;
		if (m_arrOpenTable[nMid]->total == pEle->total)
		{
			nLeft = nMid+1;//相等则插入后面减少一个移动
			break;
		}
		else if (m_arrOpenTable[nMid]->total < pEle->total)
			nRight = nMid-1;
		else
			nLeft = nMid+1;
	}
	if (nLeft < m_OpenCount)
		memmove(&m_arrOpenTable[nLeft+1],&m_arrOpenTable[nLeft],sizeof(SElement*)*(size_t(m_OpenCount)-nLeft));
	m_arrOpenTable[nLeft] = pEle;
	++m_OpenCount;
}